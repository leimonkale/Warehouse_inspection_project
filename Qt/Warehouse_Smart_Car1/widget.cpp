#include "widget.h"
#include "ui_widget.h"

#include <QtWidgets/QMessageBox>
#include <QtCore/QDateTime>
#include "monitorwidget.h"  // 确保包含这个头文件

#include <QtCharts>

using namespace Qt::StringLiterals;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    ,m_monitorWidget(nullptr)  // 初始化为nullptr
    , m_chart(nullptr)// 初始化图表指针为nullptr
    , m_temperatureSeries(nullptr)
    , m_humiditySeries(nullptr)
    , m_lightSeries(nullptr)
    , m_axisX(nullptr)
    , m_axisYTemp(nullptr)
    , m_axisYHumi(nullptr)
    , m_axisYLight(nullptr)
    , m_chartView(nullptr)
    , m_chartUpdateTimer(nullptr)
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
        parseAndUpdateSensorData(message);
    });//接收到消息时的打印

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
        + "PingResponse\n"_L1;
        ui->editLog->insertPlainText(content);
    });//心跳机制

    connect(ui->lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);//实时更改主机名
    connect(ui->spinBoxPort, QOverload<int>::of(&QSpinBox::valueChanged), this, &Widget::setClientPort);//实时更改端口号

    // 添加监控按钮的连接
    connect(ui->rtspButton, &QPushButton::clicked, this, &Widget::on_rtspButton_clicked);

    // 初始化图表
    setupChart();

    updateLogStateChange();//程序启动先打印一次
}

Widget::~Widget()
{
    delete ui;
}

/**
 * @brief 初始化图表组件
 *
 * 创建折线图、坐标轴、序列，并设置样式和布局
 */
void Widget::setupChart()
{
    // 创建折线序列
    m_temperatureSeries = new QLineSeries();
    m_humiditySeries = new QLineSeries();
    m_lightSeries = new QLineSeries();

    // 设置折线样式和名称
    m_temperatureSeries->setName("温度(°C)");
    m_temperatureSeries->setColor(Qt::red);
    m_temperatureSeries->setPen(QPen(Qt::red, 2));

    m_humiditySeries->setName("湿度(%)");
    m_humiditySeries->setColor(Qt::blue);
    m_humiditySeries->setPen(QPen(Qt::blue, 2));

    m_lightSeries->setName("光照强度");
    m_lightSeries->setColor(Qt::green);
    m_lightSeries->setPen(QPen(Qt::green, 2));

    // 创建图表 - 禁用动画效果
    m_chart = new QChart();
    m_chart->addSeries(m_temperatureSeries);
    m_chart->addSeries(m_humiditySeries);
    m_chart->addSeries(m_lightSeries);
    m_chart->setTitle("环境传感器数据实时监控");
    m_chart->setAnimationOptions(QChart::NoAnimation);  // 禁用动画
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    // 创建X轴
    m_axisX = new QValueAxis();
    m_axisX->setRange(0, MAX_DATA_POINTS);
    m_axisX->setLabelFormat("%d");
    m_axisX->setTitleText("时间(秒)");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_temperatureSeries->attachAxis(m_axisX);
    m_humiditySeries->attachAxis(m_axisX);
    m_lightSeries->attachAxis(m_axisX);

    // 创建温度Y轴
    m_axisYTemp = new QValueAxis();
    m_axisYTemp->setRange(0, 100);
    m_axisYTemp->setLabelFormat("%d");
    m_axisYTemp->setTitleText("温度(°C)");
    m_chart->addAxis(m_axisYTemp, Qt::AlignLeft);
    m_temperatureSeries->attachAxis(m_axisYTemp);

    // 创建湿度Y轴
    m_axisYHumi = new QValueAxis();
    m_axisYHumi->setRange(0, 100);
    m_axisYHumi->setLabelFormat("%d");
    m_axisYHumi->setTitleText("湿度(%)");
    m_chart->addAxis(m_axisYHumi, Qt::AlignRight);
    m_humiditySeries->attachAxis(m_axisYHumi);

    // 创建光照强度Y轴
    m_axisYLight = new QValueAxis();
    m_axisYLight->setRange(0, 100);
    m_axisYLight->setLabelFormat("%d");
    m_axisYLight->setTitleText("光照强度");
    m_chart->addAxis(m_axisYLight, Qt::AlignRight);
    m_lightSeries->attachAxis(m_axisYLight);

    // 创建图表视图 - 禁用抗锯齿
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing, false);  // 禁用抗锯齿
    m_chartView->setMinimumSize(400, 300);

    // 将图表添加到UI布局
    QVBoxLayout *chartLayout = new QVBoxLayout(ui->chartWidget);
    chartLayout->setContentsMargins(0, 0, 0, 0);
    chartLayout->addWidget(m_chartView);

    // 不预先填充任何数据点
}

void Widget::addDataToChart()
{
    // 直接添加到序列，不再使用冗余的数据容器
    m_temperatureSeries->append(m_timeCounter, m_temperature);
    m_humiditySeries->append(m_timeCounter, m_humidity);
    m_lightSeries->append(m_timeCounter, m_adcValue);

    // 清理超过60个点的旧数据
    if (m_temperatureSeries->count() > MAX_DATA_POINTS) {
        m_temperatureSeries->remove(0);
        m_humiditySeries->remove(0);
        m_lightSeries->remove(0);
    }

    // 更新X轴显示范围（滑动窗口）
    if (m_timeCounter > MAX_DATA_POINTS) {
        m_axisX->setRange(m_timeCounter - MAX_DATA_POINTS, m_timeCounter);
    } else {
        m_axisX->setRange(0, MAX_DATA_POINTS); // 保持固定范围直到数据填满
    }

    m_timeCounter++;
}

/**
 * @brief 解析传感器数据并更新成员变量
 * @param message 原始消息数据
 */
void Widget::parseAndUpdateSensorData(const QByteArray &message)
{
    QString msgStr = QString::fromUtf8(message);

    // 使用正则表达式解析数据
    QRegularExpression regex("\\{temp:(\\d+),humi:(\\d+),adc_num:(\\d+)\\}");
    QRegularExpressionMatch match = regex.match(msgStr);

    if (match.hasMatch()) {
        // 更新成员变量
        m_temperature = match.captured(1).toInt();
        m_humidity = match.captured(2).toInt();
        m_adcValue = match.captured(3).toInt();

        // 更新界面标签显示
        ui->env_lable->setText(
            QString("温度：%1  湿度：%2  光照强度：%3")
                .arg(m_temperature)
                .arg(m_humidity)
                .arg(m_adcValue)
            );

        // 添加新数据点到图表
        addDataToChart();
    }
}
// 添加监控按钮的槽函数实现
void Widget::on_rtspButton_clicked()
{
    if (!m_monitorWidget) {
        m_monitorWidget = std::make_unique<MonitorWidget>(nullptr);

        // 连接窗口关闭信号，以便重置指针
        connect(m_monitorWidget.get(), &MonitorWidget::destroyed,
                this, [this]() { m_monitorWidget.reset(); });
    }

    m_monitorWidget->show();
    m_monitorWidget->raise();  // 将窗口提到前台
    m_monitorWidget->activateWindow();  // 激活窗口
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
