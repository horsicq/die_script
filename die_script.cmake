include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XDISASMCORE_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XDisasmCore/xdisasmcore.cmake)
    set(DIE_SCRIPT_SOURCES ${DIE_SCRIPT_SOURCES} ${XDISASMCORE_SOURCES})
endif()
if (NOT DEFINED XSCANENGINE_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XScanEngine/xscanengine.cmake)
    set(DIE_SCRIPT_SOURCES ${DIE_SCRIPT_SOURCES} ${XSCANENGINE_SOURCES})
endif()
if (NOT DEFINED DETECTITEASY_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Detect-It-Easy/detectiteasy.cmake)
endif()

set(DIE_SCRIPT_SOURCES
    ${DIE_SCRIPT_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/amiga_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/amiga_script.h
    ${CMAKE_CURRENT_LIST_DIR}/archive_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/archive_script.h
    ${CMAKE_CURRENT_LIST_DIR}/binary_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/binary_script.h
    ${CMAKE_CURRENT_LIST_DIR}/com_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/com_script.h
    ${CMAKE_CURRENT_LIST_DIR}/die_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_script.h
    ${CMAKE_CURRENT_LIST_DIR}/die_scriptengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_scriptengine.h
    ${CMAKE_CURRENT_LIST_DIR}/dos16m_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dos16m_script.h
    ${CMAKE_CURRENT_LIST_DIR}/dos4g_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dos4g_script.h
    ${CMAKE_CURRENT_LIST_DIR}/elf_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/elf_script.h
    ${CMAKE_CURRENT_LIST_DIR}/global_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/global_script.h
    ${CMAKE_CURRENT_LIST_DIR}/le_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/le_script.h
    ${CMAKE_CURRENT_LIST_DIR}/lx_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lx_script.h
    ${CMAKE_CURRENT_LIST_DIR}/mach_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mach_script.h
    ${CMAKE_CURRENT_LIST_DIR}/msdos_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/msdos_script.h
    ${CMAKE_CURRENT_LIST_DIR}/ne_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ne_script.h
    ${CMAKE_CURRENT_LIST_DIR}/pe_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pe_script.h
    ${CMAKE_CURRENT_LIST_DIR}/xscriptengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xscriptengine.h
    ${CMAKE_CURRENT_LIST_DIR}/zip_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/zip_script.h
    ${CMAKE_CURRENT_LIST_DIR}/jar_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/jar_script.h
    ${CMAKE_CURRENT_LIST_DIR}/apk_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/apk_script.h
    ${CMAKE_CURRENT_LIST_DIR}/ipa_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ipa_script.h
    ${CMAKE_CURRENT_LIST_DIR}/dex_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dex_script.h
    ${CMAKE_CURRENT_LIST_DIR}/npm_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/npm_script.h
    ${CMAKE_CURRENT_LIST_DIR}/machofat_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/machofat_script.h
    ${CMAKE_CURRENT_LIST_DIR}/util_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util_script.h
    ${CMAKE_CURRENT_LIST_DIR}/javaclass_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/javaclass_script.h
    ${CMAKE_CURRENT_LIST_DIR}/pdf_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pdf_script.h
)
