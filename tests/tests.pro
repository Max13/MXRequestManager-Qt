#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T00:18:04
#
#-------------------------------------------------

QT          +=  testlib network
QT          -=  gui

TARGET      =   tst_MXRequestManager
CONFIG      +=  console
CONFIG      -=  app_bundle

TEMPLATE    =   app
LIBS        +=  -L$$shadowed(../src) -lMXRequestManager2

SOURCES     +=  tst_MXRequestManager.cpp
DEFINES     +=  SRCDIR=\\\"$$PWD/\\\"
