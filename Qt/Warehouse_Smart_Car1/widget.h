#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMqttClient>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void setClientPort(int p);

private slots:
    void on_buttonConnect_clicked();//连接服务器函数
    void on_buttonQuit_clicked();
    void updateLogStateChange();//连接状态改变，打印函数
    void brokerDisconnected();//连接断开处理函数
    void on_buttonPublish_clicked();//发布按钮函数
    void on_buttonSubscribe_clicked();//订阅按钮函数

private:
    Ui::Widget *ui;
    QMqttClient *m_client;//定义一个mqtt对象

};
#endif // WIDGET_H
