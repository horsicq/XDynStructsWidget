INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogxdynstructs.h \
    $$PWD/xdynstructsoptionswidget.h \
    $$PWD/xdynstructswidget.h

SOURCES += \
    $$PWD/dialogxdynstructs.cpp \
    $$PWD/xdynstructsoptionswidget.cpp \
    $$PWD/xdynstructswidget.cpp

FORMS += \
    $$PWD/dialogxdynstructs.ui \
    $$PWD/xdynstructsoptionswidget.ui \
    $$PWD/xdynstructswidget.ui

!contains(XCONFIG, xdynstructsengine) {
    XCONFIG += xdynstructsengine
    include($$PWD/../XDynStructsEngine/xdynstructsengine.pri)
}

!contains(XCONFIG, xhtml) {
    XCONFIG += xhtml
    include($$PWD/../Controls/xhtml.pri)
}
# TODO cmake
DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md
