#include "mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QString>
#include <QSvgRenderer>
#include <QSvgWidget>
#include <iostream>
#include <unordered_map>

#include "./ui_mainwindow.h"
#include "countydata.h"
#include "countymodel.h"
#include "csvfile.h"

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
  const QString mapPath = ":/Usa_counties_large.svg";
  ui->countyMap->load(mapPath);
  ui->countyMap->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
  connect(ui->countyMap, &SvgWidget::clicked, this, &MainWindow::mapClicked);

  // TODO: Either push this into the constructor or provide a mechanism for
  // opening from a file on the command line so this isn't always desirable.
  vData->createDefaultCounties();

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

  /*
  vCounties = ReadCsvFile(fileName.toStdString());
  if (vCounties.empty()) {
    QMessageBox::critical(
        this, "Failed to parse file",
        QString("Failed to parse the specified file %1").arg(fileName));
    return;
  }
  */
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

  // TODO: Qt doesn't have an Svg modifying class, so I think we're gonna have
  // to write out our own svg with the modifications we need...
  std::cout << "Changed " << county.name << ", "
            << AbbreviationForState(county.state) << " to "
            << (visited ? "visited" : "not visited") << "\n";
}
