QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += E:\tool\OpenCv\OpenCV-MingW-Build\include
LIBS += -LE:\tool\OpenCv\OpenCV-MingW-Build\x64\mingw\bin\

LIBS += -lopencv_core455
LIBS += -lopencv_imgproc455
LIBS += -lopencv_imgcodecs455
LIBS += -lopencv_highgui455
LIBS += -lopencv_videoio455
LIBS += -lopencv_video455
LIBS += -lopencv_videoio_ffmpeg455_64
