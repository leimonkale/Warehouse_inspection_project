#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMqttClient>
#include <monitorwidget.h>
#include <memory>
#include <QTimer>
#include <QDateTime>  // 添加DateTime头文件



QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

// 前向声明 Qt Charts 类
class QChart;
class QLineSeries;
class QValueAxis;
class QChartView;

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
    void on_rtspButton_clicked();  // 添加监控按钮的槽函数
    void parseAndUpdateSensorData(const QByteArray &message);
    //void updateChart();


private:
    Ui::Widget *ui;
    QMqttClient *m_client;//定义一个mqtt对象
    std::unique_ptr<MonitorWidget> m_monitorWidget;  // 使用智能指针

    int m_temperature = 0;    ///< 温度值
    int m_humidity = 0;       ///< 湿度值
    int m_adcValue = 0;       ///< ADC数值

    // 图表相关成员变量
    QChart *m_chart;                          ///< 图表对象
    QLineSeries *m_temperatureSeries;         ///< 温度折线序列
    QLineSeries *m_humiditySeries;            ///< 湿度折线序列
    QLineSeries *m_lightSeries;               ///< 光照强度折线序列
    QValueAxis *m_axisX;                      ///< X轴（时间轴）
    QValueAxis *m_axisYTemp;                  ///< 温度Y轴
    QValueAxis *m_axisYHumi;                  ///< 湿度Y轴
    QValueAxis *m_axisYLight;                 ///< 光照强度Y轴
    QChartView *m_chartView;                  ///< 图表视图
    QTimer *m_chartUpdateTimer;               ///< 图表更新定时器

    // // 数据存储容器
    // QVector<QPointF> m_tempData;              ///< 温度数据点集合
    // QVector<QPointF> m_humiData;              ///< 湿度数据点集合
    // QVector<QPointF> m_lightData;             ///< 光照数据点集合

    int m_timeCounter = 0;            ///< 时间计数器（单位：秒）
    //QDateTime m_startTime;  ///< 图表开始时间
    const int MAX_DATA_POINTS = 60;           ///< 最大显示数据点数（60秒数据）

    void setupChart();        ///< 初始化图表组件
    void addDataToChart();    ///< 添加新数据点到图表数据容器
    //void cleanupOldData();  // 添加清理旧数据的函数
};
#endif // WIDGET_H
