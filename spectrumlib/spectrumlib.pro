QT += core

CONFIG += c++17 staticlib
TEMPLATE = lib
TARGET = SpectrumLib

INCLUDEPATH += $$PWD
DESTDIR = $$OUT_PWD

HEADERS += \
    SpectrumDtos.h \
    spectrumservice.h

SOURCES += \
    spectrumservice.cpp
