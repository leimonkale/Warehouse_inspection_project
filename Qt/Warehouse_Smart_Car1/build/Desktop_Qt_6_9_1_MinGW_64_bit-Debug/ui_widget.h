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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *buttonQuit;
    QLabel *label;
    QPushButton *buttonConnect;
    QLabel *label_2;
    QGroupBox *groupBox;
    QPlainTextEdit *editLog;
    QSpinBox *spinBoxPort;
    QPushButton *rtspButton;
    QLabel *env_lable;
    QWidget *chartWidget;
    QLineEdit *lineEditHost;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLineEdit *lineEditTopic;
    QPushButton *buttonSubscribe;
    QHBoxLayout *horizontalLayout;
    QLabel *label_4;
    QLineEdit *lineEditMessage;
    QPushButton *buttonPublish;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(922, 517);
        buttonQuit = new QPushButton(Widget);
        buttonQuit->setObjectName("buttonQuit");
        buttonQuit->setGeometry(QRect(40, 440, 111, 41));
        label = new QLabel(Widget);
        label->setObjectName("label");
        label->setGeometry(QRect(34, 41, 30, 16));
        buttonConnect = new QPushButton(Widget);
        buttonConnect->setObjectName("buttonConnect");
        buttonConnect->setGeometry(QRect(210, 60, 80, 24));
        label_2 = new QLabel(Widget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(34, 72, 27, 16));
        groupBox = new QGroupBox(Widget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(30, 170, 291, 261));
        editLog = new QPlainTextEdit(groupBox);
        editLog->setObjectName("editLog");
        editLog->setGeometry(QRect(12, 30, 271, 221));
        spinBoxPort = new QSpinBox(Widget);
        spinBoxPort->setObjectName("spinBoxPort");
        spinBoxPort->setGeometry(QRect(70, 70, 63, 24));
        spinBoxPort->setMaximum(99999);
        spinBoxPort->setValue(1883);
        rtspButton = new QPushButton(Widget);
        rtspButton->setObjectName("rtspButton");
        rtspButton->setGeometry(QRect(170, 440, 111, 41));
        env_lable = new QLabel(Widget);
        env_lable->setObjectName("env_lable");
        env_lable->setGeometry(QRect(470, 20, 341, 31));
        QFont font;
        font.setPointSize(11);
        env_lable->setFont(font);
        chartWidget = new QWidget(Widget);
        chartWidget->setObjectName("chartWidget");
        chartWidget->setGeometry(QRect(340, 60, 571, 431));
        chartWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        lineEditHost = new QLineEdit(Widget);
        lineEditHost->setObjectName("lineEditHost");
        lineEditHost->setGeometry(QRect(70, 41, 108, 23));
        layoutWidget = new QWidget(Widget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(32, 100, 258, 72));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        horizontalLayout_2->addWidget(label_3);

        lineEditTopic = new QLineEdit(layoutWidget);
        lineEditTopic->setObjectName("lineEditTopic");

        horizontalLayout_2->addWidget(lineEditTopic);

        buttonSubscribe = new QPushButton(layoutWidget);
        buttonSubscribe->setObjectName("buttonSubscribe");

        horizontalLayout_2->addWidget(buttonSubscribe);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        horizontalLayout->addWidget(label_4);

        lineEditMessage = new QLineEdit(layoutWidget);
        lineEditMessage->setObjectName("lineEditMessage");

        horizontalLayout->addWidget(lineEditMessage);

        buttonPublish = new QPushButton(layoutWidget);
        buttonPublish->setObjectName("buttonPublish");

        horizontalLayout->addWidget(buttonPublish);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        buttonQuit->setText(QCoreApplication::translate("Widget", "Quit", nullptr));
        label->setText(QCoreApplication::translate("Widget", "Host:", nullptr));
        buttonConnect->setText(QCoreApplication::translate("Widget", "Connect", nullptr));
        label_2->setText(QCoreApplication::translate("Widget", "Port:", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Widget", "Log Messages", nullptr));
        rtspButton->setText(QCoreApplication::translate("Widget", "\347\233\221\346\216\247", nullptr));
        env_lable->setText(QCoreApplication::translate("Widget", "\346\270\251\345\272\246\357\274\2320    \346\271\277\345\272\246\357\274\2320    \345\205\211\347\205\247\345\274\272\345\272\246\357\274\2320", nullptr));
        lineEditHost->setText(QCoreApplication::translate("Widget", "broker.emqx.io", nullptr));
        lineEditHost->setPlaceholderText(QString());
        label_3->setText(QCoreApplication::translate("Widget", "Topic:", nullptr));
        lineEditTopic->setText(QCoreApplication::translate("Widget", "mochen9227", nullptr));
        buttonSubscribe->setText(QCoreApplication::translate("Widget", "Subscribe", nullptr));
        label_4->setText(QCoreApplication::translate("Widget", "Message:", nullptr));
        lineEditMessage->setText(QCoreApplication::translate("Widget", "{temp:23,humi:70,adc_num:50}", nullptr));
        buttonPublish->setText(QCoreApplication::translate("Widget", "Publish", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
