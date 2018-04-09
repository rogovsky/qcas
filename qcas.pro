#
#

TEMPLATE = lib
CONFIG += qt warn_on exceptions release dll
QT     += network

VVERSION = $$[QT_VERSION]
isEmpty(VVERSION) {
  # Qt 3
  CONFIG += staticlib

  DESTDIR = lib_qt3
  MOC_DIR = .moc_qt3
  OBJECTS_DIR = .obj_qt3
} else {
  # Qt 4/5/...
  CONFIG += staticlib

  DESTDIR = lib
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

VERSION = 1.0
TARGET = qcas

INCLUDEPATH += inc
LIBS        +=

#Input
HEADERS += inc/cas.h
SOURCES += src/cas.cpp

OTHER_FILES += \
    README.md
