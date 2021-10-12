#pragma once

#include <QSvgWidget>

class SvgWidget : public QSvgWidget
{
  Q_OBJECT
  void mousePressEvent(QMouseEvent* e) override { emit clicked(e); }

signals:
  void clicked(QMouseEvent*);
};
