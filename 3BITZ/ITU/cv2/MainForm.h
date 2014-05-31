#ifndef MAIN_FORM__H
#define MAIN_FORM__H

#include <QtCore>
#include "ui_untitled.h"

class MainForm : public QWidget {
  Q_OBJECT

  private:
    Ui::Form ui;

  private slots:
    void computeRes();

  public:
    MainForm ( QWidget * parent = 0, Qt::WindowFlags f = 0 );

  signals:
    void done(QString str);

};

#endif // MAIN_FORM__H
