#ifndef MYSTATEWIDGET_H
#define MYSTATEWIDGET_H

#include <QtCore>
#include <QtGui>

class MyStateWidget : public QWidget {
  Q_OBJECT

  public:
    MyStateWidget(QWidget *parent = 0);
    ~MyStateWidget();

  public slots:
    void newState(QString value);

  protected:
    void paintEvent(QPaintEvent* event);

    bool stateMem;

};

#endif // MYSTATEWIDGET_H
