INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialogxdynstructs.ui \
    $$PWD/xdynstructswidget.ui

HEADERS += \
    $$PWD/dialogxdynstructs.h \
    $$PWD/xdynstructswidget.h

SOURCES += \
    $$PWD/dialogxdynstructs.cpp \
    $$PWD/xdynstructswidget.cpp

!contains(XCONFIG, xdynstructsengine) {
    XCONFIG += xdynstructsengine
    include($$PWD/../XDynStructsEngine/xdynstructsengine.pri)
}

!contains(XCONFIG, xhtml) {
    XCONFIG += xhtml
    include($$PWD/../Controls/xhtml.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md
