
FORMS += mainForm.ui 
HEADERS += pracaoc.h \
    blockblast.h
SOURCES += pracaoc.cpp \
           blockblast.cpp \
           main.cpp
TEMPLATE = app
QT += core gui opengl

QMAKE_CXXFLAGS_DEBUG += -fno-omit-frame-pointer
QMAKE_CXXFLAGS_RELEASE += -fno-omit-frame-pointer
QMAKE_CFLAGS_DEBUG += -fno-omit-frame-pointer
QMAKE_CFLAGS_RELEASE += -fno-omit-frame-pointer

TARGET = pracAOC
