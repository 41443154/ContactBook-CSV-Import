
QT += widgets

CONFIG += c++17

# Project name
TARGET = ContactBook

# Source files
SOURCES += \
    main.cpp \
    mywidget.cpp

# Header files
HEADERS += \
    mywidget.h

# UI files
FORMS += \
    mywidget.ui

# UTF-8 encoding settings for MinGW
win32-g++ {
    QMAKE_CFLAGS += -finput-charset=UTF-8
    QMAKE_CXXFLAGS += -finput-charset=UTF-8
    QMAKE_CXXFLAGS += -fexec-charset=UTF-8
}

# Default deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
