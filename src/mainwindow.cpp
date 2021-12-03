#include "mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QString>
#include <QSvgRenderer>
#include <QSvgWidget>
#include <iostream>
#include <unordered_map>

#include "./ui_mainwindow.h"
#include "countydata.h"

constexpr auto countyMapResource = ":/Usa_counties_large.svg";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setWindowTitle("QCounties");

  // File Menu
  connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
  connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSave);

  connect(ui->actionQuit, &QAction::triggered, qApp, &QGuiApplication::quit,
          Qt::QueuedConnection);

  // View Menu
  connect(ui->actionZoom_In, &QAction::triggered, this, &MainWindow::zoomMax);
  connect(ui->actionZoom_Out, &QAction::triggered, this, &MainWindow::zoomFit);

  // Data View
  ui->dataSplitter->setSizes({1, 500});

  // SVG Widget
  ui->countyMap->load(QString(countyMapResource));
  ui->countyMap->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
  connect(ui->countyMap, &SvgWidget::clicked, this, &MainWindow::mapClicked);

  // Model setup
  vModel = new QStandardItemModel(this);
  vModel->setColumnCount(2);
  std::unordered_map<State, QStandardItem*> states;
  // TODO: Could implement partial checking and the ability to check/uncheck
  // entire states, but for now there's no visited column for the states.
  for (State state : AllStates()) {
    auto stateItem = new QStandardItem(QString(AbbreviationForState(state)));
    stateItem->setEditable(false);
    states[state] = stateItem;
    vModel->invisibleRootItem()->appendRow(stateItem);
  }

  for (size_t i = 0; i < vData->size(); i++) {
    const std::pair<County, bool>& countyPair = vData->mCounties[i];
    const County& county = countyPair.first;
    const bool visited = countyPair.second;

    QStandardItem* countyItem = new QStandardItem(QString(county.name.c_str()));
    countyItem->setEditable(false);

    QStandardItem* visitedItem = new QStandardItem(1);
    visitedItem->setCheckable(true);
    visitedItem->setCheckState(visited ? Qt::Checked : Qt::Unchecked);
    visitedItem->setData(QVariant::fromValue(i), Qt::UserRole);

    QList<QStandardItem*> items = {countyItem, visitedItem};
    states[county.state]->appendRow(items);
  }


  vModel->setHorizontalHeaderItem(0, new QStandardItem(QString("County")));
  vModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Visited")));
  ui->countyList->setModel(vModel);

  connect(vModel, &QStandardItemModel::itemChanged, this,
          &MainWindow::countyChanged);
}


MainWindow::~MainWindow() { delete ui; }


void MainWindow::onOpen()
{
  const QString fileName = QFileDialog::getOpenFileName(this, "Open File");
  if (fileName.isEmpty()) return;

  const bool ok = vData->readFromFile(fileName.toStdString());
  if (!ok) {
    // TODO: Report here instead of the command line
    QMessageBox::critical(
        this, "Failed to parse file",
        QString("Failed to parse the specified file %1").arg(fileName));
  }
  rebuildSvgFromData();
}

void MainWindow::onSave() {}

void MainWindow::zoomFit() { ui->countyMap->setMinimumSize({0, 0}); }
void MainWindow::zoomMax() { ui->countyMap->setMinimumSize({2970, 1881}); }

void MainWindow::mapClicked(const QMouseEvent* e)
{
  const QPoint p = e->pos();

  std::cout << "click { " << p.x() << ", " << p.y() << " }\n";
}

void MainWindow::countyChanged(const QStandardItem* item)
{
  assert(item->column() == 1);
  assert(item->parent() != vModel->invisibleRootItem());

  const size_t idx = item->data(Qt::UserRole).toUInt();
  std::pair<County, bool>& countyPair = vData->mCounties[idx];
  County& county = countyPair.first;
  bool& visited = countyPair.second;

  visited = (item->checkState() == Qt::Checked);

  vData->setCountyVisited(county, visited);

  rebuildSvgFromData();
}

// TODO: There's gotta be a better way to do this.  Is there anything in new Qt
// about Svg manipulation?
void MainWindow::rebuildSvgFromData()
{
  QFile f(countyMapResource);
  assert(f.open(QFile::ReadOnly));

  // TODO: The compiler hangs when I use QTemporaryFile, what's going on
  // here...?
  QFile::remove("/tmp/fix-this");
  QFile newMap("/tmp/fix-this");

  assert(newMap.open(QFile::ReadWrite));
  QTextStream newMapStream(&newMap);

  while (!f.atEnd()) {
    const QString line = f.readLine().data();
    QString replacementLine = line;
    if (line.contains("path")) {
      QRegularExpression rx;
      rx.setPattern("id=\"([^\"]*,[^\"]*)\"");
      const QRegularExpressionMatch match = rx.match(line);
      if (match.hasMatch()) {
        const QString countyStr = match.captured(1);
        const County county = County::fromString(countyStr.toStdString());
        assert(county.state != State::NStates);
        if (vData->countyVisited(county)) {
          QRegularExpression addColor;
          addColor.setPattern("/>");
          QString withColor = " fill=\"blue\" />";
          replacementLine.replace(addColor, withColor);
        }
      }
    }
    newMapStream << replacementLine;
  }

  newMap.close();
  ui->countyMap->load(newMap.fileName());
  ui->countyMap->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
}
