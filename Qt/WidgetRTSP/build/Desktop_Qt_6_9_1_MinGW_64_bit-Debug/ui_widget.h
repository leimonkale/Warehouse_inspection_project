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
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLabel *videoLabel;
    QTextEdit *statusText;
    QLineEdit *urlEdit;
    QLabel *urlLabel;
    QPushButton *startButton;
    QPushButton *stopButton;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(487, 562);
        videoLabel = new QLabel(Widget);
        videoLabel->setObjectName("videoLabel");
        videoLabel->setGeometry(QRect(20, 110, 411, 291));
        videoLabel->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"));
        statusText = new QTextEdit(Widget);
        statusText->setObjectName("statusText");
        statusText->setGeometry(QRect(20, 420, 421, 111));
        urlEdit = new QLineEdit(Widget);
        urlEdit->setObjectName("urlEdit");
        urlEdit->setGeometry(QRect(30, 40, 321, 21));
        urlLabel = new QLabel(Widget);
        urlLabel->setObjectName("urlLabel");
        urlLabel->setGeometry(QRect(30, 20, 71, 21));
        startButton = new QPushButton(Widget);
        startButton->setObjectName("startButton");
        startButton->setGeometry(QRect(30, 70, 81, 24));
        stopButton = new QPushButton(Widget);
        stopButton->setObjectName("stopButton");
        stopButton->setGeometry(QRect(130, 70, 81, 24));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        videoLabel->setText(QString());
        urlLabel->setText(QCoreApplication::translate("Widget", "RTSP URL:", nullptr));
        startButton->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245RTSP", nullptr));
        stopButton->setText(QCoreApplication::translate("Widget", "\345\201\234\346\255\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
