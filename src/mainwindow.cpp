#include "mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QString>
#include <QStringList>
#include <QSvgRenderer>
#include <QSvgWidget>
#include <QTextStream>
#include <iostream>

#include "./ui_mainwindow.h"
#include "county.h"
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

  createDefaultCounties();
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

void MainWindow::mapClicked(QMouseEvent* e)
{
  const QPoint p = e->pos();

  std::cout << "click { " << p.x() << ", " << p.y() << " }\n";
}


// Fill default county data from the resource
void MainWindow::createDefaultCounties()
{
  vCounties.clear();

  QFile f(":/counties.csv");

  if (f.open(QIODevice::ReadOnly)) {
    QTextStream s(&f);
    while (!s.atEnd()) {
      const QStringList line = s.readLine().split(",");
      assert(line.size() == 2);
      const std::string name = line[0].toStdString();
      const std::string stateAbbr = line[1].trimmed().toStdString();
      State state = State::NStates;
      for (State candidateState : AllStates()) {
        if (stateAbbr == AbbreviationForState(candidateState)) {
          state = candidateState;
          break;
        }
      }
      assert(state != State::NStates);

      vCounties.push_back({name, state});
    }
  }
}
