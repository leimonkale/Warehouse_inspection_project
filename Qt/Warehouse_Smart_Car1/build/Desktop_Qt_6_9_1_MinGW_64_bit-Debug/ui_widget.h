/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *buttonPublish;
    QLineEdit *lineEditHost;
    QPushButton *buttonQuit;
    QLabel *label;
    QPushButton *buttonConnect;
    QLineEdit *lineEditMessage;
    QLabel *label_3;
    QPushButton *buttonSubscribe;
    QLabel *label_2;
    QLabel *label_4;
    QGroupBox *groupBox;
    QPlainTextEdit *editLog;
    QLineEdit *lineEditTopic;
    QSpinBox *spinBoxPort;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(800, 600);
        buttonPublish = new QPushButton(Widget);
        buttonPublish->setObjectName("buttonPublish");
        buttonPublish->setGeometry(QRect(204, 140, 80, 24));
        lineEditHost = new QLineEdit(Widget);
        lineEditHost->setObjectName("lineEditHost");
        lineEditHost->setGeometry(QRect(70, 35, 108, 23));
        buttonQuit = new QPushButton(Widget);
        buttonQuit->setObjectName("buttonQuit");
        buttonQuit->setGeometry(QRect(77, 431, 80, 24));
        label = new QLabel(Widget);
        label->setObjectName("label");
        label->setGeometry(QRect(34, 35, 30, 16));
        buttonConnect = new QPushButton(Widget);
        buttonConnect->setObjectName("buttonConnect");
        buttonConnect->setGeometry(QRect(185, 52, 80, 24));
        lineEditMessage = new QLineEdit(Widget);
        lineEditMessage->setObjectName("lineEditMessage");
        lineEditMessage->setGeometry(QRect(90, 140, 108, 23));
        label_3 = new QLabel(Widget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(30, 110, 33, 16));
        buttonSubscribe = new QPushButton(Widget);
        buttonSubscribe->setObjectName("buttonSubscribe");
        buttonSubscribe->setGeometry(QRect(204, 110, 80, 24));
        label_2 = new QLabel(Widget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(34, 70, 27, 16));
        label_4 = new QLabel(Widget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(30, 140, 54, 16));
        groupBox = new QGroupBox(Widget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(30, 190, 280, 234));
        editLog = new QPlainTextEdit(groupBox);
        editLog->setObjectName("editLog");
        editLog->setGeometry(QRect(12, 30, 256, 192));
        lineEditTopic = new QLineEdit(Widget);
        lineEditTopic->setObjectName("lineEditTopic");
        lineEditTopic->setGeometry(QRect(90, 110, 108, 23));
        spinBoxPort = new QSpinBox(Widget);
        spinBoxPort->setObjectName("spinBoxPort");
        spinBoxPort->setGeometry(QRect(70, 70, 63, 24));
        spinBoxPort->setMaximum(99999);
        spinBoxPort->setValue(1883);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        buttonPublish->setText(QCoreApplication::translate("Widget", "Publish", nullptr));
        lineEditHost->setText(QCoreApplication::translate("Widget", "broker.emqx.io", nullptr));
        lineEditHost->setPlaceholderText(QString());
        buttonQuit->setText(QCoreApplication::translate("Widget", "Quit", nullptr));
        label->setText(QCoreApplication::translate("Widget", "Host:", nullptr));
        buttonConnect->setText(QCoreApplication::translate("Widget", "Connect", nullptr));
        lineEditMessage->setText(QCoreApplication::translate("Widget", "This is a test message", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "Topic:", nullptr));
        buttonSubscribe->setText(QCoreApplication::translate("Widget", "Subscribe", nullptr));
        label_2->setText(QCoreApplication::translate("Widget", "Port:", nullptr));
        label_4->setText(QCoreApplication::translate("Widget", "Message:", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Widget", "Log Messages", nullptr));
        lineEditTopic->setText(QCoreApplication::translate("Widget", "zeng kewu", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
