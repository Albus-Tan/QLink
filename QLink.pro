# QTest
TEMPLATE = app
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
QT += testlib


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    character.cpp \
    gameoverwidget.cpp \
    global.cpp \
    main.cpp \
    map.cpp \
    menu.cpp \
    progressbar.cpp \
    props.cpp \
    simpletest.cpp \
    utils.cpp \
    widget.cpp

HEADERS += \
    character.h \
    gameoverwidget.h \
    global.h \
    map.h \
    menu.h \
    progressbar.h \
    props.h \
    simpletest.h \
    utils.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Bugs \
    Problems
