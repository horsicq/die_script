lessThan(QT_MAJOR_VERSION, 6): QT += script
greaterThan(QT_MAJOR_VERSION, 5): QT += qml

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/binary_script.h \
    $$PWD/die_script.h \
    $$PWD/die_scriptengine.h \
    $$PWD/msdos_script.h \
    $$PWD/pe_script.h \
    $$PWD/ne_script.h \
    $$PWD/le_script.h \
    $$PWD/mach_script.h \
    $$PWD/elf_script.h

SOURCES += \
    $$PWD/binary_script.cpp \
    $$PWD/die_script.cpp \
    $$PWD/die_scriptengine.cpp \
    $$PWD/msdos_script.cpp \
    $$PWD/pe_script.cpp \
    $$PWD/ne_script.cpp \
    $$PWD/le_script.cpp \
    $$PWD/mach_script.cpp \
    $$PWD/elf_script.cpp

!contains(XCONFIG, xformats) {
    XCONFIG += xformats
    include($$PWD/../Formats/xformats.pri)
}

!contains(XCONFIG, xarchive) {
    XCONFIG += xarchive
    include($$PWD/../XArchive/xarchive.pri)
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
    $$PWD/die_script.cmake
