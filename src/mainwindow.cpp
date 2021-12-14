#include "mainwindow.h"

#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QScreen>
#include <QStandardItemModel>
#include <QString>
#include <QSvgRenderer>
#include <QSvgWidget>
#include <iostream>
#include <unordered_map>

#include "./ui_mainwindow.h"
#include "countydata.h"

static constexpr auto countyMapResource = ":/Usa_counties_large.svg";

static constexpr int NameColumn = 0;
static constexpr int VisitedColumn = 1;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setWindowTitle("QCounties");

  const QSize primaryScreenSize = QGuiApplication::screens()[0]->size();
  resize(primaryScreenSize.width() * 0.6, primaryScreenSize.height() * 0.7);

  // File Menu
  connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
  connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSave);
  connect(ui->actionExportSvg, &QAction::triggered, this,
          &MainWindow::onExportSvg);

  connect(ui->actionQuit, &QAction::triggered, qApp, &QGuiApplication::quit,
          Qt::QueuedConnection);

  // Edit Menu
  connect(ui->actionChangeColor, &QAction::triggered, this,
          &MainWindow::onColorChange);


  // View Menu
  connect(ui->actionZoom_In, &QAction::triggered, this, &MainWindow::zoomMax);
  connect(ui->actionZoom_Out, &QAction::triggered, this, &MainWindow::zoomFit);

  // Data View
  ui->dataSplitter->setSizes({600, 1600});

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
    QStandardItem* stateItem =
        new QStandardItem(QString(AbbreviationForState(state)));
    stateItem->setEditable(false);
    states[state] = stateItem;

    QStandardItem* visitedItem = new QStandardItem(VisitedColumn);
    visitedItem->setEditable(false);

    QList<QStandardItem*> items = {stateItem, visitedItem};

    vModel->invisibleRootItem()->appendRow(items);
  }

  for (size_t i = 0; i < vData->size(); i++) {
    const std::pair<County, bool>& countyPair = vData->mCounties[i];
    const County& county = countyPair.first;
    const bool visited = countyPair.second;

    QStandardItem* countyItem = new QStandardItem(QString(county.name.c_str()));
    countyItem->setEditable(false);

    QStandardItem* visitedItem = new QStandardItem(VisitedColumn);
    visitedItem->setEditable(false);
    visitedItem->setCheckable(true);
    visitedItem->setCheckState(visited ? Qt::Checked : Qt::Unchecked);
    visitedItem->setData(QVariant::fromValue(i), Qt::UserRole);

    QList<QStandardItem*> items = {countyItem, visitedItem};
    states[county.state]->appendRow(items);
  }


  vModel->setHorizontalHeaderItem(NameColumn,
                                  new QStandardItem(QString("County")));
  vModel->setHorizontalHeaderItem(VisitedColumn,
                                  new QStandardItem(QString("Visited")));
  ui->countyList->setModel(vModel);
  ui->countyList->header()->resizeSection(0, 150);
  ui->countyList->header()->resizeSection(1, 10);

  connect(vModel, &QStandardItemModel::itemChanged, this,
          &MainWindow::countyChanged);
}


MainWindow::~MainWindow() { delete ui; }


void MainWindow::onOpen()
{
  const QString fileName = QFileDialog::getOpenFileName(
      this, "Open File", {}, "Comma-separated Values (*.csv)");
  if (fileName.isEmpty()) return;

  const bool ok = vData->readFromFile(fileName.toStdString());
  if (!ok) {
    // TODO: Report here instead of the command line
    QMessageBox::critical(
        this, "Failed to parse file",
        QString("Failed to parse the specified file %1").arg(fileName));
  }
  rebuildFromData();
}

void MainWindow::onSave()
{
  const QString fileName = QFileDialog::getSaveFileName(
      this, "Export CSV", {}, "Comma-separated Values (*.csv)");
  if (fileName.isEmpty()) return;

  const bool ok = vData->writeToFile(fileName.toStdString());

  if (!ok) {
    QMessageBox::critical(
        this, "Failed to save CSV",
        QString("Failed to export the CSV to file %1").arg(fileName));
  }
}

void MainWindow::onExportSvg()
{
  const QString fileName = QFileDialog::getSaveFileName(
      this, "Export SVG", {}, "Scalable Vector Graphic (*.svg)");
  if (fileName.isEmpty()) return;

  const bool ok = vData->writeSvg(fileName.toStdString());
  if (!ok) {
    QMessageBox::critical(
        this, "Failed to save SVG",
        QString("Failed to export the SVG to file %1").arg(fileName));
  }
}


void MainWindow::onColorChange()
{
  const QColor color = QColorDialog::getColor(Qt::blue, this, "Pick a color");
  if (!color.isValid()) return;
  vData->setSvgColor(color.name().toStdString());
  rebuildSvgFromData();
}

void MainWindow::zoomFit() { ui->countyMap->setMinimumSize({0, 0}); }
void MainWindow::zoomMax() { ui->countyMap->setMinimumSize({2970, 1881}); }

void MainWindow::mapClicked(const QMouseEvent* e)
{
  const QPoint p = e->pos();

  std::cout << "click { " << p.x() << ", " << p.y() << " }\n";
}

void MainWindow::countyChanged(const QStandardItem* item)
{
  if (vLoadingFromData) return;

  assert(item->column() == 1);
  assert(item->parent() != vModel->invisibleRootItem());

  const size_t idx = item->data(Qt::UserRole).toUInt();
  std::pair<County, bool>& countyPair = vData->mCounties[idx];
  County& county = countyPair.first;
  bool& visited = countyPair.second;

  visited = (item->checkState() == Qt::Checked);

  vData->setCountyVisited(county, visited);

  rebuildSvgFromData();
  generateStatistics();
}


void MainWindow::rebuildFromData()
{
  rebuildModelFromData();
  rebuildSvgFromData();
  generateStatistics();
}


void MainWindow::rebuildModelFromData()
{
  const bool loading = vLoadingFromData;
  vLoadingFromData = true;

  // This could be more surgical.
  std::function<void(const QModelIndex&)> updateEachChild =
      [this, &updateEachChild](const QModelIndex& parent) {
        for (int i = 0; i < vModel->rowCount(parent); i++) {
          // Need the name column because only the 0th column has children
          const QModelIndex idx = vModel->index(i, NameColumn, parent);
          const QModelIndex visitedIdx = idx.sibling(idx.row(), VisitedColumn);

          bool ok = false;
          const size_t dataIdx = visitedIdx.data(Qt::UserRole).toUInt(&ok);
          if (ok) {
            const bool visited = vData->mCounties[dataIdx].second;
            vModel->itemFromIndex(visitedIdx)
                ->setCheckState(visited ? Qt::Checked : Qt::Unchecked);
          }

          if (vModel->hasChildren(idx)) {
            updateEachChild(idx);
          }
        }
      };

  // start with the invisible root item
  updateEachChild({});

  vLoadingFromData = loading;
}


void MainWindow::rebuildSvgFromData()
{
  const std::string svg = vData->svg();
  assert(!svg.empty());

  ui->countyMap->load(QByteArray(svg.c_str()));
  ui->countyMap->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
}


void MainWindow::generateStatistics()
{
  const std::pair<size_t, double> countiesCompleted =
      vData->numberAndPercentVisited();

  ui->countiesCompleted->setText(QString("%1").arg(countiesCompleted.first));

  ui->countiesCompletedPercent->setText(
      QString("%1%").arg(countiesCompleted.second));
}
