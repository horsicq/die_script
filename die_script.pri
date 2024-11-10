QT       += concurrent

lessThan(QT_MAJOR_VERSION, 6): QT += script
greaterThan(QT_MAJOR_VERSION, 5): QT += qml

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/amiga_script.h \
    $$PWD/apk_script.h \
    $$PWD/archive_script.h \
    $$PWD/binary_script.h \
    $$PWD/com_script.h \
    $$PWD/dex_script.h \
    $$PWD/die_script.h \
    $$PWD/die_scriptengine.h \
    $$PWD/dos16m_script.h \
    $$PWD/elf_script.h \
    $$PWD/global_script.h \
    $$PWD/ipa_script.h \
    $$PWD/jar_script.h \
    $$PWD/le_script.h \
    $$PWD/lx_script.h \
    $$PWD/mach_script.h \
    $$PWD/msdos_script.h \
    $$PWD/ne_script.h \
    $$PWD/pe_script.h \
    $$PWD/npm_script.h \
    $$PWD/util_script.h \
    $$PWD/xscriptengine.h \
    $$PWD/zip_script.h

SOURCES += \
    $$PWD/amiga_script.cpp \
    $$PWD/apk_script.cpp \
    $$PWD/archive_script.cpp \
    $$PWD/binary_script.cpp \
    $$PWD/com_script.cpp \
    $$PWD/dex_script.cpp \
    $$PWD/die_script.cpp \
    $$PWD/die_scriptengine.cpp \
    $$PWD/dos16m_script.cpp \
    $$PWD/elf_script.cpp \
    $$PWD/global_script.cpp \
    $$PWD/ipa_script.cpp \
    $$PWD/jar_script.cpp \
    $$PWD/le_script.cpp \
    $$PWD/lx_script.cpp \
    $$PWD/mach_script.cpp \
    $$PWD/msdos_script.cpp \
    $$PWD/ne_script.cpp \
    $$PWD/pe_script.cpp \
    $$PWD/npm_script.cpp \
    $$PWD/util_script.cpp \
    $$PWD/xscriptengine.cpp \
    $$PWD/zip_script.cpp

!contains(XCONFIG, xscanengine) {
    XCONFIG += xscanengine
    include($$PWD/../XScanEngine/xscanengine.pri)
}

contains(XCONFIG, use_capstone_x86) {
    !contains(XCONFIG, xcapstone_x86) {
        XCONFIG += xcapstone_x86
        include($$PWD/../XCapstone/xcapstone_x86.pri)
    }
}

!contains(XCONFIG, use_capstone_x86) {
    !contains(XCONFIG, xcapstone) {
        XCONFIG += xcapstone
        include($$PWD/../XCapstone/xcapstone.pri)
    }
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/die_script.cmake
