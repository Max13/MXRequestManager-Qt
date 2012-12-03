#-------------------------------------------------
#
# Project created by QtCreator 2012-07-20T12:21:32
#
#-------------------------------------------------

QT			-= gui network

VERSION		= 0.2.55

TARGET		= mxrequestmanager
TEMPLATE	= lib
CONFIG		+= staticlib

SOURCES		+= MXRequestManager.cpp \
			   json.cpp
HEADERS		+= MXRequestManager.hpp \
			   json.h

INSTALLS	+= targethead
INSTALLS	+= target

# Installation directives: make install/uninstall
targethead.path	= $$[QT_INSTALL_PREFIX]/include/$$TARGET
targethead.files = $$HEADERS
target.path = $$[QT_INSTALL_PREFIX]/lib
