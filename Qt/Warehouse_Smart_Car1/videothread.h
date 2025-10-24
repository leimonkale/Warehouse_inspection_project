#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>
#include <QMutex>
#include <QPixmap>
#include "opencv2/opencv.hpp"

/**
 * @brief 视频捕获线程类
 *
 * 负责在后台线程中捕获RTSP视频流，并将视频帧转换为QPixmap供界面显示
 * 使用OpenCV进行视频流处理，通过信号槽机制与界面通信
 */
class VideoThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoThread(QObject *parent = nullptr);
    ~VideoThread();

    void startCapture(const QString &url);// 开始捕获指定URL的视频流
    void stopCapture();                   // 停止视频捕获
    QPixmap getCurrentFrame();             // 获取当前视频帧的QPixmap

signals:
    void frameReady();                      // 新帧就绪信号
    void errorOccurred(const QString &error); // 错误发生信号

protected:
    void run() override;

private:
    QString m_url;              ///< RTSP视频流URL
    QMutex m_mutex;             ///< 线程互斥锁，保护共享数据
    bool m_stop = false;         ///< 线程停止标志
    cv::VideoCapture m_capture; ///< OpenCV视频捕获对象
    cv::Mat m_frame;            ///< 当前视频帧(OpenCV格式)
    QPixmap m_pixmap;           ///< 当前视频帧(QPixmap格式)
};

#endif // VIDEOTHREAD_H
