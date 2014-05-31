#include <QtGui>
#include <iostream>
#include "mystatewidget.h"

MyStateWidget::MyStateWidget(QWidget* parent) : QWidget(parent) {
}

MyStateWidget::~MyStateWidget() {
}

void MyStateWidget::newState(QString newVal) {
  std::cout << "hello world" << std::endl;
  update();
}

void MyStateWidget::paintEvent(QPaintEvent* ) {
  QPainter painter(this);
  painter.setPen(QRgb(0x0000ee00));
  QSize s = size();

  std::cout << "width: " << s.width() << " height: " << s.height() << std::endl;
/*
     QPoint minuteHand[3] = {
         QPoint(7, 8),
         QPoint(-7, 8),
         QPoint(0, -70)
     };
     painter.drawConvexPolygon(minuteHand, 3);
     painter.drawEllipse();
   */
      painter.drawRect(0, 0, 10, 10);
}
