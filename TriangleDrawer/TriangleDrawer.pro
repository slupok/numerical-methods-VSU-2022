QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../eigen

SOURCES += \
    cuthillmckeeutils.cpp \
    loader.cpp \
    main.cpp \
    mainwindow.cpp \
    stiffnessutils.cpp \
    triangulationutils.cpp \
    viewport.cpp

HEADERS += \
    cuthillmckeeutils.h \
    loader.h \
    mainwindow.h \
    stiffnessutils.h \
    triangulationutils.h \
    viewport.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
