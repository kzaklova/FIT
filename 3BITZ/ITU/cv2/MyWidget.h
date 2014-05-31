#ifndef MYWIDGET_H
#define MYWIDGET_H


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

#endif // MYWIDGET_H
