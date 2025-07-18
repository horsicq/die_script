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
    $$PWD/cfbf_script.h \
    $$PWD/com_script.h \
    $$PWD/dex_script.h \
    $$PWD/die_script.h \
    $$PWD/die_scriptengine.h \
    $$PWD/dos16m_script.h \
    $$PWD/dos4g_script.h \
    $$PWD/elf_script.h \
    $$PWD/global_script.h \
    $$PWD/image_script.h \
    $$PWD/ipa_script.h \
    $$PWD/javaclass_script.h \
    $$PWD/jar_script.h \
    $$PWD/jpeg_script.h \
    $$PWD/le_script.h \
    $$PWD/lx_script.h \
    $$PWD/mach_script.h \
    $$PWD/msdos_script.h \
    $$PWD/ne_script.h \
    $$PWD/pe_script.h \
    $$PWD/pdf_script.h \
    $$PWD/npm_script.h \
    $$PWD/rar_script.h \
    $$PWD/machofat_script.h \
    $$PWD/util_script.h \
    $$PWD/xscriptengine.h \
    $$PWD/zip_script.h

SOURCES += \
    $$PWD/amiga_script.cpp \
    $$PWD/apk_script.cpp \
    $$PWD/archive_script.cpp \
    $$PWD/binary_script.cpp \
    $$PWD/cfbf_script.cpp \
    $$PWD/com_script.cpp \
    $$PWD/dex_script.cpp \
    $$PWD/die_script.cpp \
    $$PWD/die_scriptengine.cpp \
    $$PWD/dos16m_script.cpp \
    $$PWD/dos4g_script.cpp \
    $$PWD/elf_script.cpp \
    $$PWD/global_script.cpp \
    $$PWD/image_script.cpp \
    $$PWD/ipa_script.cpp \
    $$PWD/javaclass_script.cpp \
    $$PWD/jar_script.cpp \
    $$PWD/jpeg_script.cpp \
    $$PWD/le_script.cpp \
    $$PWD/lx_script.cpp \
    $$PWD/mach_script.cpp \
    $$PWD/msdos_script.cpp \
    $$PWD/ne_script.cpp \
    $$PWD/pe_script.cpp \
    $$PWD/pdf_script.cpp \
    $$PWD/npm_script.cpp \
    $$PWD/rar_script.cpp \
    $$PWD/machofat_script.cpp \
    $$PWD/util_script.cpp \
    $$PWD/xscriptengine.cpp \
    $$PWD/zip_script.cpp

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
