#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QWidget>
#include "videothread.h"

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorWidget(QWidget *parent = nullptr);
    ~MonitorWidget();

private slots:
    void onStartRtspTest();                 ///< 开始RTSP测试按钮点击
    void onFrameReady();                     ///< 视频帧就绪处理
    void onVideoError(const QString &error); ///< 视频错误处理
    void stopCapture();                     ///< 停止视频捕获

private:
    Ui::MonitorWidget *ui;
    VideoThread *m_videoThread;             ///< 视频捕获线程对象
    bool m_isCapturing;                     ///< 捕获状态标志

    void setupUI();
};

#endif // MONITORWIDGET_H
