#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include"opencv2/opencv.hpp"
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>

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

private slots:
    void onStartRtspTest();           // 开始RTSP测试
    void onUpdateFrame();             // 更新视频帧
    //void testOpenCVBasicFunctions();


private:
    Ui::Widget *ui;
    QTimer *timer;                    // 定时器用于更新视频帧
    cv::VideoCapture capture;         // OpenCV视频捕获对象
    bool isCapturing;                 // 是否正在捕获视频
    cv::Mat currentFrame;             // 当前帧

    void setupUI();                   // 初始化UI
    void stopCapture();               // 停止视频捕获
};
#endif // WIDGET_H
