#include "widget.h"
#include "ui_widget.h"

#include "opencv2/opencv.hpp"
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QGroupBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , timer(new QTimer(this))
    , isCapturing(false)
{
    ui->setupUi(this);

    setupUI();

    connect(timer, &QTimer::timeout, this, &Widget::onUpdateFrame);

    qDebug() << "OpenCV Version:" << CV_VERSION;

    // 测试OpenCV基本功能
    cv::Mat testMat = cv::Mat::zeros(100, 100, CV_8UC1);
    qDebug() << "OpenCV Mat created, size:" << testMat.rows << "x" << testMat.cols;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupUI()
{

    ui->urlEdit->setPlaceholderText("例如: rtsp://admin:password@192.168.1.100:554/stream1");
    // 提供多个测试URL
    QStringList testUrls = {
        "rtsp://192.168.18.97:554/mjpeg/1"
    };

    ui->urlEdit->setText(testUrls[0]);

    ui->statusText->setReadOnly(true);
    ui->statusText->setMaximumHeight(150);

    ui->videoLabel->setStyleSheet("border: 1px solid gray; background-color: black;");
    ui->videoLabel->setText("视频显示区域");

    // 连接按钮信号
    connect(ui->startButton, &QPushButton::clicked, this, [=]() {
        QString rtspUrl = ui->urlEdit->text().trimmed();
        if (rtspUrl.isEmpty()) {
            QMessageBox::warning(this, "警告", "请输入RTSP URL");
            return;
        }

        ui->statusText->append("正在连接: " + rtspUrl);
        onStartRtspTest();
    });


    connect(ui->stopButton, &QPushButton::clicked, this, [=]() {
        stopCapture();
        ui->statusText->append("已停止RTSP流");
        ui->videoLabel->setPixmap(QPixmap());
        ui->videoLabel->setText("视频显示区域");
    });
}

void Widget::onStartRtspTest()
{
    if (isCapturing) {
        stopCapture();
    }

    QString rtspUrl = ui->urlEdit->text().trimmed();

    ui->statusText->append("尝试打开视频流...");
    ui->statusText->append("URL: " + rtspUrl);

    try {
        // 尝试不同的方法打开视频流
        bool opened = false;

        // 方法1: 直接打开
        capture.open(rtspUrl.toStdString());

        if (!capture.isOpened()) {
            ui->statusText->append("错误: 所有方法都无法打开视频流");
            ui->statusText->append("可能的原因:");
            ui->statusText->append("1. OpenCV未编译FFmpeg/GStreamer支持");
            ui->statusText->append("2. 网络连接问题或URL错误");
            ui->statusText->append("3. 防火墙阻止了连接");
            ui->statusText->append("4. 缺少必要的解码器");

            QMessageBox::critical(this, "错误",
                                  "无法打开视频流。\n"
                                  "请检查:\n"
                                  "1. URL是否正确\n"
                                  "2. 网络连接\n"
                                  "3. OpenCV是否支持视频流\n"
                                  "4. 尝试使用HTTP链接测试");
            return;
        }

        isCapturing = true;
        ui->statusText->append("成功打开视频流!");

        // 获取视频信息
        double fps = capture.get(cv::CAP_PROP_FPS);
        int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
        double totalFrames = capture.get(cv::CAP_PROP_FRAME_COUNT);

        ui->statusText->append(QString("视频信息: %1x%2, FPS: %3, 总帧数: %4")
                                   .arg(width).arg(height).arg(fps).arg(totalFrames));

        // 启动定时器读取帧
        int interval = 1000 / (fps > 0 ? fps : 30); // 默认30fps
        timer->start(interval);
        ui->statusText->append(QString("定时器间隔: %1ms").arg(interval));

    } catch (const cv::Exception& e) {
        ui->statusText->append("OpenCV异常: " + QString(e.what()));
        QMessageBox::critical(this, "OpenCV错误", QString("OpenCV异常: %1").arg(e.what()));
    } catch (const std::exception& e) {
        ui->statusText->append("标准异常: " + QString(e.what()));
        QMessageBox::critical(this, "错误", QString("标准异常: %1").arg(e.what()));
    }
}

void Widget::onUpdateFrame()
{
    if (!isCapturing || !capture.isOpened()) {
        timer->stop();
        return;
    }

    try {
        cv::Mat frame;
        bool success = capture.read(frame);

        if (success && !frame.empty()) {
            currentFrame = frame.clone();

            // 将OpenCV Mat转换为QImage并显示
            cv::Mat displayFrame;
            if (frame.channels() == 1) {
                cv::cvtColor(frame, displayFrame, cv::COLOR_GRAY2RGB);
            } else {
                cv::cvtColor(frame, displayFrame, cv::COLOR_BGR2RGB);
            }

            QImage img(displayFrame.data, displayFrame.cols, displayFrame.rows,
                       displayFrame.step, QImage::Format_RGB888);

            QPixmap pixmap = QPixmap::fromImage(img);

            if (!pixmap.isNull()) {
                // 缩放以适应显示区域，保持宽高比
                QLabel *videoLabel = ui->videoLabel;
                pixmap = pixmap.scaled(videoLabel->width(), videoLabel->height(),
                                       Qt::KeepAspectRatio, Qt::SmoothTransformation);
                videoLabel->setPixmap(pixmap);
            }

            static int frameCount = 0;
            frameCount++;
            if (frameCount % 30 == 0) { // 每30帧更新一次状态
                ui->statusText->append(QString("已处理 %1 帧").arg(frameCount));
            }
        } else {
            ui->statusText->append("错误: 无法读取视频帧或帧为空");
            stopCapture();
        }
    } catch (const cv::Exception& e) {
        ui->statusText->append("读取帧时发生OpenCV错误: " + QString(e.what()));
        stopCapture();
    } catch (const std::exception& e) {
        ui->statusText->append("读取帧时发生标准错误: " + QString(e.what()));
        stopCapture();
    }
}

void Widget::stopCapture()
{
    if (timer->isActive()) {
        timer->stop();
    }

    if (capture.isOpened()) {
        capture.release();
    }

    isCapturing = false;
    ui->statusText->append("视频捕获已停止");
}
