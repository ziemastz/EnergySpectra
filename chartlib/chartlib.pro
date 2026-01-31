QT += charts widgets

CONFIG += c++17 staticlib
TEMPLATE = lib
TARGET = ChartLib

INCLUDEPATH += $$PWD
DESTDIR = $$OUT_PWD

HEADERS += \
    zoomablechartview.h \
    spectrumchart.h

SOURCES += \
    zoomablechartview.cpp \
    spectrumchart.cpp
