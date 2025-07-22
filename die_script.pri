QT       += concurrent

lessThan(QT_MAJOR_VERSION, 6): QT += script
greaterThan(QT_MAJOR_VERSION, 5): QT += qml

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/die_script.h \
    $$PWD/die_scriptengine.h \
    $$PWD/global_script.h \
    $$PWD/util_script.h \
    $$PWD/xscriptengine.h 

SOURCES += \
    $$PWD/die_script.cpp \
    $$PWD/die_scriptengine.cpp \
    $$PWD/global_script.cpp \
    $$PWD/util_script.cpp \
    $$PWD/xscriptengine.cpp

!contains(XCONFIG, xscanengine) {
    XCONFIG += xscanengine
    include($$PWD/../XScanEngine/xscanengine.pri)
}

!contains(XCONFIG, xdisasmcore) {
    XCONFIG += xdisasmcore
    include($$PWD/../XDisasmCore/xdisasmcore.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/die_script.cmake
