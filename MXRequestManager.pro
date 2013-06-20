#-------------------------------------------------
#
# Project created by QtCreator 2012-07-20T12:21:32
#
#-------------------------------------------------

QT			+= widgets network

VER_MAJ     =   2
VER_MIN     =   2
VER_BLD     =   $$cat(BUILD, lines)
VERSION     =   $${VER_MAJ}.$${VER_MIN}.$${VER_BLD}

TARGET		= MXRequestManager2
TEMPLATE	= lib
CONFIG		+= staticlib

SOURCES		+= MXRequestManager.cpp
HEADERS		+= MXRequestManager.hpp

INSTALLS	+= targethead
INSTALLS	+= target

# Installation directives: make install/uninstall
targethead.path	= $$[QT_INSTALL_PREFIX]/include/$$TARGET
targethead.files = $$HEADERS
target.path = $$[QT_INSTALL_PREFIX]/lib
