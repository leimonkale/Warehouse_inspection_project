QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camer.cpp \
    connect_server.cpp \
    main.cpp \
    map_node.cpp \
    recv_date.cpp \
    warehouse_inspection.cpp

HEADERS += \
    camer.h \
    connect_server.h \
    map_node.h \
    recv_date.h \
    warehouse_inspection.h

FORMS += \
    warehouse_inspection.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
