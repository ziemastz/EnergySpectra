QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = EnergySpectra
DESTDIR = $$OUT_PWD/../release

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += ../chartlib ../spectrumlib ..
LIBS += -L$$OUT_PWD/../chartlib -lChartLib \
        -L$$OUT_PWD/../spectrumlib -lSpectrumLib

# resources
RESOURCES += resources.qrc

# exe icon (Windows)
win32:RC_ICONS += resources/app.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
