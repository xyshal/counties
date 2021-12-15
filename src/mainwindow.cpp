#include "mainwindow.h"

#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRegularExpression>
#include <QScreen>
#include <QStandardItemModel>
#include <QString>
#include <QSvgRenderer>
#include <QSvgWidget>
#include <exception>
#include <iostream>
#include <unordered_map>

#include "./ui_mainwindow.h"
#include "config.h"
#include "countydata.h"
#include "preferences.h"

static constexpr auto CountyMapResource = ":/Usa_counties_large.svg";

static constexpr int NameColumn = 0;
static constexpr int VisitedColumn = 1;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  // Keep headers off the interface, since that seems to be the Qt way.
  vData = std::make_unique<CountyData>();
  vPreferences = std::make_unique<Preferences>();

  // Preserve color choice
  vData->setSvgColor(vPreferences->mVisitedColor);

  ui->setupUi(this);
  setWindowTitle(QString("%1 %2").arg(PROJECT_NAME).arg(PROJECT_VER));

  const QSize primaryScreenSize = QGuiApplication::screens()[0]->size();
  resize(primaryScreenSize.width() * 0.6, primaryScreenSize.height() * 0.7);

  // File Menu
  connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
  connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSave);
  connect(ui->actionExportSvg, &QAction::triggered, this,
          &MainWindow::onExportSvg);
  connect(ui->actionExportPng, &QAction::triggered, this,
          &MainWindow::onExportPng);

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
  ui->countyMap->load(QString(CountyMapResource));
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
    visitedItem->setText("0 (0%)");
    visitedItem->setEditable(false);
    visitedItem->setCheckable(true);
    visitedItem->setCheckState(Qt::Unchecked);
    visitedItem->setData(QVariant::fromValue(state), Qt::UserRole);

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

  const bool ok = [this,fileName]() {
    try { // The CSV parsing library uses exceptions
      return vData->readFromFile(fileName.toStdString());
    } catch (const std::exception& e) {
      std::cout << e.what() << "\n";
      return false;
    }
  }();

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


void MainWindow::onExportPng()
{
  const QString fileName = QFileDialog::getSaveFileName(
      this, "Export PNG", {}, "Portable Network Graphics (*.png)");
  if (fileName.isEmpty()) return;

  QImage image(5940, 3762, QImage::Format_ARGB32);
  image.fill(QColor("white"));
  // TODO: Upgrade Qt
  // image.fill(QColorConstants::Svg::white);

  QPainter painter(&image);
  ui->countyMap->renderer()->render(&painter);

  const bool ok = image.save(fileName);
  if (!ok) {
    QMessageBox::critical(
        this, "Failed to save PNG",
        QString("Failed to export the PNG to file %1").arg(fileName));
  }
}


void MainWindow::onColorChange()
{
  const QColor color = QColorDialog::getColor(Qt::blue, this, "Pick a color");
  if (!color.isValid()) return;
  const std::string colorName = color.name().toStdString();
  vData->setSvgColor(colorName);
  vPreferences->mVisitedColor = colorName;

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
  // Top-level stats
  const Statistics stats = vData->statistics();

  ui->countiesCompleted->setText(QString("%1").arg(stats.countiesVisited));

  ui->countiesCompletedPercent->setText(
      QString("%1%").arg(stats.percentCountiesVisited));

  ui->statesCompleted->setText(QString("%1").arg(stats.statesCompleted));

  ui->statesCompletedPercent->setText(
      QString("%1%").arg(stats.percentStatesCompleted));

  // State-level stats
  const bool loading = vLoadingFromData;
  vLoadingFromData = true;

  const QModelIndex invisibleRootItem;
  const int nRows = vModel->rowCount(invisibleRootItem);
  for (int i = 0; i < nRows; i++) {
    QModelIndex idx = vModel->index(i, VisitedColumn, invisibleRootItem);
    bool ok = false;
    const State state = static_cast<State>(idx.data(Qt::UserRole).toUInt(&ok));
    if (ok) {
      const std::pair<size_t, double>& pair =
          stats.countiesAndPercentCompletePerState.at(state);
      QStandardItem* item = vModel->itemFromIndex(idx);
      item->setText(QString("%1 (%2%)").arg(pair.first).arg(pair.second));
      if (pair.second == 100.0) {
        // TODO: Is this just confusing?  Should this be a mechanism for
        // marking all the counties as visited or not visited, with a
        // mixed-value state?
        item->setCheckable(true);
        item->setCheckState(Qt::Checked);
      } else {
        item->setCheckState(Qt::Unchecked);
        item->setCheckable(false);
      }
    }
  }

  vLoadingFromData = loading;
}
