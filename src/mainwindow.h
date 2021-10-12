#pragma once

#include <QMainWindow>

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

  void mapClicked(QMouseEvent*);

private:
  Ui::MainWindow* ui;
};
