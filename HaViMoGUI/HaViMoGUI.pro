TEMPLATE = app
TARGET = HaViMoGUI
QT += core \
    gui \
    widgets
HEADERS += src/CamImageView.h \
    src/DataLinkDirectConn.h \
    src/DataLinkRoboBuilder.h \
    src/DataLinkCM5.h \
    src/portcombobox.h \
    src/DXL/dxl_hal.h \
    src/DXL/dynamixel.h \
    src/DataLink.h \
    src/DataLinkUSB2Dyn.h \
    src/LUTView.h \
    src/havimogui.h
SOURCES += src/CamImageView.cpp \
    src/DataLinkDirectConn.cpp \
    src/DataLinkRoboBuilder.cpp \
    src/DataLinkCM5.cpp \
    src/DXL/dxl_hal.c \
    src/DXL/dynamixel.c \
    src/DataLink.cpp \
    src/DataLinkUSB2Dyn.cpp \
    src/LUTView.cpp \
    src/havimogui.cpp \
    src/main.cpp \
    src/portcombobox.cpp
FORMS += havimogui.ui
RESOURCES += 
LIBS +=
