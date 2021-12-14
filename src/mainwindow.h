#pragma once

#include <QMainWindow>
#include <memory>

class CountyData;
class Preferences;
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

  MainWindow(const MainWindow&) = delete;
  MainWindow(MainWindow&&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;
  MainWindow& operator=(MainWindow&&) = delete;

private slots:
  void onOpen();
  void onSave();
  void onExportSvg();
  void onColorChange();
  void zoomFit();
  void zoomMax();

  void mapClicked(const QMouseEvent*);
  void countyChanged(const QStandardItem*);

private:
  void rebuildFromData();
  void rebuildModelFromData();
  void rebuildSvgFromData();
  void generateStatistics();

private:
  Ui::MainWindow* ui;

  QStandardItemModel* vModel;
  std::unique_ptr<CountyData> vData;
  std::unique_ptr<Preferences> vPreferences;

  // TODO: Isn't there a better way to do this these days?
  bool vLoadingFromData = false;
};
