#include "mainwindow.h"

#include <QFileDialog>
#include <QMouseEvent>
#include <QString>
#include <QSvgRenderer>
#include <QSvgWidget>
#include <iostream>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setWindowTitle("QCounties");

  // File
  connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
  connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSave);

  connect(ui->actionQuit, &QAction::triggered, qApp, &QGuiApplication::quit,
          Qt::QueuedConnection);

  // View
  connect(ui->actionZoom_In, &QAction::triggered, this, &MainWindow::zoomMax);
  connect(ui->actionZoom_Out, &QAction::triggered, this, &MainWindow::zoomFit);

  // SVG Widget
  const QString mapPath = ":/Usa_counties_large.svg";
  ui->countyMap->load(mapPath);
  ui->countyMap->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
  connect(ui->countyMap, &SvgWidget::clicked, this, &MainWindow::mapClicked);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onOpen()
{
  const QString fileName = QFileDialog::getOpenFileName(this, "Open File");
  assert(QFileInfo(fileName).exists());
}

void MainWindow::onSave() {}

void MainWindow::zoomFit() { ui->countyMap->setMinimumSize({0, 0}); }
void MainWindow::zoomMax() { ui->countyMap->setMinimumSize({2970, 1881}); }

void MainWindow::mapClicked(QMouseEvent* e)
{
  const QPoint p = e->pos();

  std::cout << "click { " << p.x() << ", " << p.y() << " }\n";
}
