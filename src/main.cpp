#include <QApplication>

#include "mainwindow.h"

int main(int argc, char** argv)
{
  QApplication a(argc, argv);
  a.setApplicationName("QCounties");

  MainWindow w;
  w.show();

  return a.exec();
}
