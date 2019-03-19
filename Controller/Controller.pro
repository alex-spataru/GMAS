#-------------------------------------------------------------------------------
# Opciones de compilacion
#-------------------------------------------------------------------------------

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

CONFIG += c++11

#-------------------------------------------------------------------------------
# Configuracion de Qt
#-------------------------------------------------------------------------------

TEMPLATE = app
TARGET = gmas

QT += xml
QT += svg
QT += core
QT += quick
QT += charts
QT += widgets
QT += serialport
QT += quickcontrols2

QTPLUGIN += qsvg

#-------------------------------------------------------------------------------
# Importar codigo fuente
#-------------------------------------------------------------------------------

HEADERS += \
    src/Serial.h

SOURCES += \
    src/main.cpp \
    src/Serial.cpp

RESOURCES += \
    assets/assets.qrc
