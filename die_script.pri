QT       += script

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/binary_script.h \
    $$PWD/die_script.h \
    $$PWD/die_scriptengine.h \
    $$PWD/msdos_script.h \
    $$PWD/pe_script.h \
    $$PWD/mach_script.h \
    $$PWD/elf_script.h

SOURCES += \
    $$PWD/binary_script.cpp \
    $$PWD/die_script.cpp \
    $$PWD/die_scriptengine.cpp \
    $$PWD/msdos_script.cpp \
    $$PWD/pe_script.cpp \
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
