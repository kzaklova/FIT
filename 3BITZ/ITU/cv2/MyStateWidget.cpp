#include <QtGui>
#include <iostream>
#include "MyStateWidget.h"

MyStateWidget::MyStateWidget(QWidget* parent) : QWidget(parent) {
}

MyStateWidget::~MyStateWidget() {
}

void MyStateWidget::newState(QString newVal) {
  if (newVal == "NDF")
      stateMem = false;
  else
      stateMem = true;
  update();
}

void MyStateWidget::paintEvent(QPaintEvent* ) {
  QPainter painter(this);
  QSize s = size();

  painter.setPen(QRgb(0xcc000000));
  painter.setBrush(QBrush(QRgb(0xcc000000)));
  painter.drawRect(0, 0, s.width()-1, s.height()-1);

  if (stateMem) {
      painter.setPen(QRgb(0x00cccc00));
      painter.setBrush(QBrush(QRgb(0x00cccc00)));
  }
  else {
      painter.setPen(QRgb(0x000000cc));
      painter.setBrush(QBrush(QRgb(0x000000cc)));
  }


  //std::cout << "width: " << s.width() << " height: " << s.height() << std::endl;

     /*QPoint minuteHand[3] = {
         QPoint(1, s.height()-1),
         QPoint(s.width()/2, 1),
         QPoint(s.width()-1, s.height()-1)
     };
     painter.drawConvexPolygon(minuteHand, 3);*/

  QPoint minuteHand[21] = {
      QPoint(s.width()/2, s.height()-1),
      QPoint(s.width()/16*7, s.height()/8*6),
      QPoint(s.width()/8*3, s.height()-10),
      QPoint(s.width()/16*5, s.height()/8*6),
      QPoint(s.width()/4, s.height()-5),
      QPoint(0,s.height()/2),
      QPoint(s.width()/4, 0),
      QPoint(s.width()/4+25, 30),
      QPoint(s.width()/4+60, 30),
      QPoint(s.width()/2-15, 0),
      QPoint(s.width()/2-5, 7),

      QPoint(s.width()/2+5, 7),
      QPoint(s.width()/2+15, 0),
      QPoint(s.width()/4*3-60,30),
      QPoint(s.width()/4*3-25,30),
      QPoint(s.width()/4*3,0),
      QPoint(s.width()-1,s.height()/2),
      QPoint(s.width()/4*3, s.height()-5),
      QPoint(s.width()/16*11, s.height()/8*6),
      QPoint(s.width()/8*5, s.height()-10),
      QPoint(s.width()/16*9, s.height()/8*6)

     };

  painter.drawPolygon(minuteHand,21);

  //painter.drawRect(0, 0, s.width()-1, s.height()-1);
  //painter.drawEllipse(0, 0, s.width()-1, s.height()-1);
}
