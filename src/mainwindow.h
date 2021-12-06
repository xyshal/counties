#pragma once

#include <QMainWindow>
#include <memory>

#include "countydata.h"

class QStandardItem;
class QStandardItemModel;

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  void onOpen();
  void onSave();
  void zoomFit();
  void zoomMax();

  void mapClicked(const QMouseEvent*);
  void countyChanged(const QStandardItem*);

private:
  void rebuildFromData();
  void rebuildModelFromData();
  void rebuildSvgFromData();

private:
  Ui::MainWindow* ui;
  QStandardItemModel* vModel;
  std::shared_ptr<CountyData> vData = std::make_shared<CountyData>();

  // TODO: Isn't there a better way to do this these days?
  bool vLoadingFromData = false;
};
