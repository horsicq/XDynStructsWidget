INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!contains(XCONFIG, allformatwidgets) {
    XCONFIG += allformatwidgets
    include($$PWD/../XDynStructsEngine/xdynstructsengine.pri)
}
