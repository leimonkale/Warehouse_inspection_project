#include "monitorwidget.h"
#include "ui_monitorwidget.h"
#include <QMessageBox>
#include <QVBoxLayout>

MonitorWidget::MonitorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MonitorWidget)
    , m_videoThread(new VideoThread(this)) // 创建视频线程，指定父对象自动管理内存
    , m_isCapturing(false)                  // 初始状态为未捕获
{
    ui->setupUi(this);
    setupUI();

    // 连接视频线程信号
    connect(m_videoThread, &VideoThread::frameReady, this, &MonitorWidget::onFrameReady);
    connect(m_videoThread, &VideoThread::errorOccurred, this, &MonitorWidget::onVideoError);

    // 连接按钮信号
    connect(ui->startButton, &QPushButton::clicked, this, &MonitorWidget::onStartRtspTest);
    connect(ui->stopButton, &QPushButton::clicked, this, &MonitorWidget::stopCapture);
}

MonitorWidget::~MonitorWidget()
{
    stopCapture();
    delete ui;
}

void MonitorWidget::setupUI()
{
    ui->urlEdit->setPlaceholderText("例如: rtsp://admin:password@192.168.1.100:554/stream1");

    // 设置测试URL
    QStringList testUrls = {
        "rtsp://192.168.180.231:554/mjpeg/1"
    };
    ui->urlEdit->setText(testUrls[0]);

    ui->statusText->setReadOnly(true);
    ui->videoLabel->setStyleSheet("border: 1px solid gray; background-color: black;");
    ui->videoLabel->setText("视频显示区域");
    ui->videoLabel->setAlignment(Qt::AlignCenter);
}
/**
 * @brief 开始RTSP测试按钮点击处理
 *
 * 验证URL输入，启动视频捕获线程
 */
void MonitorWidget::onStartRtspTest()
{
    // if (m_isCapturing) {
    //     stopCapture();
    // }

    QString rtspUrl = ui->urlEdit->text().trimmed();
    if (rtspUrl.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入RTSP URL");
        return;
    }

    ui->statusText->append("正在连接: " + rtspUrl);
    m_videoThread->startCapture(rtspUrl);// 启动视频捕获线程
    m_isCapturing = true;// 更新捕获状态
}
/**
 * @brief 视频帧就绪处理槽函数
 *
 * 从视频线程获取当前帧并显示在界面上
 */
void MonitorWidget::onFrameReady()
{
    // 从视频线程获取当前帧的QPixmap
    QPixmap pixmap = m_videoThread->getCurrentFrame();
    if (!pixmap.isNull()) {
        // 缩放显示
        QPixmap scaledPixmap = pixmap.scaled(ui->videoLabel->width(),
                                             ui->videoLabel->height(),
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
        ui->videoLabel->setPixmap(scaledPixmap);// 在视频标签上显示缩放后的图片
    }
}

/**
 * @brief 视频错误处理槽函数
 * @param error 错误信息
 *
 * 显示错误信息，如果是连接错误则自动停止捕获
 */
void MonitorWidget::onVideoError(const QString &error)
{
    ui->statusText->append(error);
    if (error.startsWith("无法打开")) {
        stopCapture();
    }
}
/**
 * @brief 停止视频捕获
 *
 * 停止视频线程，重置界面状态
 */
void MonitorWidget::stopCapture()
{
    m_videoThread->stopCapture();// 停止视频捕获线程
    m_isCapturing = false; // 更新捕获状态
    ui->statusText->append("视频捕获已停止");
    ui->videoLabel->setPixmap(QPixmap());// 清空显示的图片
    ui->videoLabel->setText("视频显示区域");// 恢复初始文本
}
