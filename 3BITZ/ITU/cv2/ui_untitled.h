/********************************************************************************
** Form generated from reading UI file 'untitled.ui'
**
** Created: Mon Oct 8 16:33:01 2012
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UNTITLED_H
#define UI_UNTITLED_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "MyStateWidget.h"

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *operand1;
    QLineEdit *operation;
    QLineEdit *operand2;
    QLineEdit *result;
    QPushButton *PushButton;
    MyStateWidget *widget;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(400, 300);
        verticalLayout = new QVBoxLayout(Form);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        operand1 = new QLineEdit(Form);
        operand1->setObjectName(QString::fromUtf8("operand1"));

        verticalLayout_2->addWidget(operand1);

        operation = new QLineEdit(Form);
        operation->setObjectName(QString::fromUtf8("operation"));

        verticalLayout_2->addWidget(operation);

        operand2 = new QLineEdit(Form);
        operand2->setObjectName(QString::fromUtf8("operand2"));

        verticalLayout_2->addWidget(operand2);

        result = new QLineEdit(Form);
        result->setObjectName(QString::fromUtf8("result"));

        verticalLayout_2->addWidget(result);

        PushButton = new QPushButton(Form);
        PushButton->setObjectName(QString::fromUtf8("PushButton"));

        verticalLayout_2->addWidget(PushButton);

        widget = new MyStateWidget(Form);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(50, 50));

        verticalLayout_2->addWidget(widget);


        verticalLayout->addLayout(verticalLayout_2);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
        PushButton->setText(QApplication::translate("Form", "Do some magic", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UNTITLED_H
