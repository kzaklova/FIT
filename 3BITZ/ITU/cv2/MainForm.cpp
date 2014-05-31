#include <QtCore>
#include <iostream>
#include "MainForm.h"

MainForm::MainForm(QWidget * parent, Qt::WindowFlags f) : QWidget ( parent,f ) {
  ui.setupUi(this);

  connect(this, SIGNAL(done(QString)), ui.result, SLOT(setText(QString)));
  connect(ui.PushButton, SIGNAL(clicked()), this, SLOT(computeRes()));
  connect(this, SIGNAL(done(QString)), ui.widget, SLOT(newState(QString)));

}

void MainForm::computeRes() {
    bool err1 = true, err2 = true;
    float num1 = ui.operand1->text().toFloat(&err1);
    float num2 = ui.operand2->text().toFloat(&err2);
    QString oper = ui.operation->text();

    if (!err1 || !err2)
        emit(done(QString("NDF")));

    else {
        if (oper == QString("+")) {
            emit(done(QString("%1").arg(num1+num2)));
        }
        else if (oper == QString("-")) {
            emit(done(QString("%1").arg(num1-num2)));
        }
        else if (oper == QString("*")) {
            emit(done(QString("%1").arg(num1*num2)));
        }
        else if (oper == QString("/")) {
            if (num2 == 0)
                emit(done(QString("NDF")));
            else
                emit(done(QString("%1").arg(num1/num2)));
        }
        else
            emit(done(QString("NDF")));
    }
}
