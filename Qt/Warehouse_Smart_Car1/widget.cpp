#include "widget.h"
#include "ui_widget.h"

#include <QtWidgets/QMessageBox>
#include <QtCore/QDateTime>

using namespace Qt::StringLiterals;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_client = new QMqttClient(this);
    m_client->setHostname(ui->lineEditHost->text());
    m_client->setPort(static_cast<quint16>(ui->spinBoxPort->value()));

    connect(m_client, &QMqttClient::stateChanged, this, &Widget::updateLogStateChange);//连接状态改变，调用打印
    connect(m_client, &QMqttClient::disconnected, this, &Widget::brokerDisconnected);//连接断开处理

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
        + " Received Topic: "_L1
            + topic.name()
            + " Message: "_L1
            + message
            + u'\n';
        ui->editLog->insertPlainText(content);
    });//接收到消息时的打印

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
        + "PingResponse\n"_L1;
        ui->editLog->insertPlainText(content);
    });//心跳机制

    connect(ui->lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);//实时更改主机名
    connect(ui->spinBoxPort, QOverload<int>::of(&QSpinBox::valueChanged), this, &Widget::setClientPort);//实时更改端口号
    updateLogStateChange();//程序启动先打印一次
}

Widget::~Widget()
{
    delete ui;
}

//连接服务器函数
void Widget::on_buttonConnect_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPort->setEnabled(false);
        ui->buttonConnect->setText(tr("Disconnect"));
        m_client->connectToHost();
    } else {
        ui->lineEditHost->setEnabled(true);
        ui->spinBoxPort->setEnabled(true);
        ui->buttonConnect->setText(tr("Connect"));
        m_client->disconnectFromHost();
    }
}

void Widget::on_buttonQuit_clicked()
{
    QApplication::quit();
}

//连接状态改变，打印函数
void Widget::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
    + ": State Change"_L1
        + QString::number(m_client->state())
        + u'\n';
    ui->editLog->insertPlainText(content);
}

//连接断开处理函数
void Widget::brokerDisconnected()
{
    ui->lineEditHost->setEnabled(true);
    ui->spinBoxPort->setEnabled(true);
    ui->buttonConnect->setText(tr("Connect"));
}

//端口设置函数
void Widget::setClientPort(int p)
{
    m_client->setPort(static_cast<quint16>(p));
}

//发布按钮函数
void Widget::on_buttonPublish_clicked()
{
    if (m_client->publish(ui->lineEditTopic->text(), ui->lineEditMessage->text().toUtf8()) == -1)
        QMessageBox::critical(this, u"Error"_s, u"Could not publish message"_s);
}

//订阅按钮函数
void Widget::on_buttonSubscribe_clicked()
{
    auto subscription = m_client->subscribe(ui->lineEditTopic->text());
    if (!subscription) {
        QMessageBox::critical(this, u"Error"_s,
                              u"Could not subscribe. Is there a valid connection?"_s);
        return;
    }
}
