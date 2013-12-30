#-------------------------------------------------
#
# Project created by QtCreator 2012-07-20T12:21:32
#
#-------------------------------------------------

QT			+= network

VER_MAJ     =   2
VER_MIN     =   2
VER_BLD     =   $$cat(BUILD, lines)
VERSION     =   $${VER_MAJ}.$${VER_MIN}.$${VER_BLD}

win32:system(cmd /C scripts\get_build.bat)
else:system(scripts/get_build.bash --next)

TARGET		= MXRequestManager2
TEMPLATE	= lib
CONFIG		+= staticlib

SOURCES		+= MXRequestManager.cpp
HEADERS		+= MXRequestManager.hpp

CONFIG(debug, debug|release):  DEFINES += QT_NO_DEBUG_OUTPUT

INSTALLS	+= targethead
INSTALLS	+= target

# Installation directives: make install/uninstall
targethead.path	= $$[QT_INSTALL_PREFIX]/include/$$TARGET
targethead.files = $$HEADERS
target.path = $$[QT_INSTALL_PREFIX]/lib
