INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/binary_script.h \
    $$PWD/die_script.h \
    $$PWD/die_scriptengine.h \
    $$PWD/msdos_script.h \
    $$PWD/pe_script.h \
    $$PWD/text_script.h \
    $$PWD/mach_script.h \
    $$PWD/elf_script.h

SOURCES += \
    $$PWD/binary_script.cpp \
    $$PWD/die_script.cpp \
    $$PWD/die_scriptengine.cpp \
    $$PWD/msdos_script.cpp \
    $$PWD/pe_script.cpp \
    $$PWD/text_script.cpp \
    $$PWD/mach_script.cpp \
    $$PWD/elf_script.cpp

!contains(XCONFIG, xpe) {
    XCONFIG += xpe
    include(../Formats/xpe.pri)
}

!contains(XCONFIG, xmsdos) {
    XCONFIG += xmsdos
    include(../Formats/xmsdos.pri)
}

!contains(XCONFIG, xelf) {
    XCONFIG += xelf
    include(../Formats/xelf.pri)
}

!contains(XCONFIG, xmach) {
    XCONFIG += xmach
    include(../Formats/xmach.pri)
}

