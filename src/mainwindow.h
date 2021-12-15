/*
 * Copyright (C) 2021 Owen Schandle
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
  void onExportPng();
  void onColorChange();
  void zoomFit();
  void zoomMax();
  void onAbout();

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
