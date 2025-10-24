#include "videothread.h"
#include <QDebug>
#include <QElapsedTimer>

VideoThread::VideoThread(QObject *parent)
    : QThread(parent)
{
}

VideoThread::~VideoThread()
{
    stopCapture();
}

/**
 * @brief 开始视频捕获
 * @param url RTSP视频流URL
 *
 * 设置视频流URL并启动工作线程
 */
void VideoThread::startCapture(const QString &url)
{
    m_url = url;        // 保存视频流URL
    m_stop = false;     // 重置停止标志
    start();             // 启动QThread线程
}

/**
 * @brief 停止视频捕获
 *
 * 设置停止标志，等待线程结束，并释放视频捕获资源
 */
void VideoThread::stopCapture()
{
    m_stop = true; // 设置停止标志
    if (isRunning()) {
        wait(1000); // 等待线程结束，最多1秒
    }
    // 释放OpenCV视频捕获资源
    if (m_capture.isOpened()) {
        m_capture.release();
    }
}
/**
 * @brief 获取当前视频帧
 * @return 当前帧的QPixmap对象
 *
 * 线程安全地获取当前视频帧，使用互斥锁保护共享数据
 */
QPixmap VideoThread::getCurrentFrame()
{
    QMutexLocker locker(&m_mutex);  // 自动加锁，函数结束时自动解锁
    return m_pixmap;                // 返回当前帧的QPixmap
}

/**
 * @brief 线程主循环
 *
 * 视频捕获的核心逻辑：
 * 1. 打开RTSP视频流
 * 2. 循环读取视频帧
 * 3. 转换图像格式
 * 4. 控制帧率
 * 5. 发送信号通知界面更新
 */
void VideoThread::run()
{
    // 打开视频流
    m_capture.open(m_url.toStdString());
    if (!m_capture.isOpened()) {
        emit errorOccurred("无法打开视频流: " + m_url);
        return;
    }

    emit errorOccurred("成功打开视频流");

    // 设置视频捕获参数，减少延迟
    m_capture.set(cv::CAP_PROP_BUFFERSIZE, 1);// 设置缓冲区大小为1，减少延迟
    m_capture.set(cv::CAP_PROP_FPS, 30);

    cv::Mat frame;          // 临时存储读取的帧
    QElapsedTimer timer;    // 用于帧率控制的计时器
    int targetMsPerFrame = 33; // 约30fps

    while (!m_stop) {  // 3. 主循环：持续读取视频帧直到停止标志为true
        timer.start();

        //读取一帧数据
        if (!m_capture.read(frame) || frame.empty()) {
            msleep(10); // 短暂休眠避免CPU占用过高
            continue;
        }

        // 转换图像格式：OpenCV Mat → QPixmap
        cv::Mat displayFrame;
        if (frame.channels() == 1) {
            // 灰度图转RGB
            cv::cvtColor(frame, displayFrame, cv::COLOR_GRAY2RGB);
        } else {
            // BGR转RGB（OpenCV默认格式为BGR）
            cv::cvtColor(frame, displayFrame, cv::COLOR_BGR2RGB);
        }

        // 创建QImage并转换为QPixmap
        QImage img(displayFrame.data, displayFrame.cols, displayFrame.rows,
                   displayFrame.step, QImage::Format_RGB888);

        QPixmap pixmap = QPixmap::fromImage(img);

         // 5. 更新当前帧（线程安全）
        {
            QMutexLocker locker(&m_mutex);
            m_pixmap = pixmap;
        }

        // 6. 发送帧就绪信号，通知界面更新
        emit frameReady();

        // 控制帧率
        int elapsed = timer.elapsed();
        if (elapsed < targetMsPerFrame) {
            msleep(targetMsPerFrame - elapsed);
        }
    }

    // 8. 循环结束，释放视频捕获资源
    if (m_capture.isOpened()) {
        m_capture.release();
    }
}
