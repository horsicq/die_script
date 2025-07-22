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
    ${CMAKE_CURRENT_LIST_DIR}/die_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_script.h
    ${CMAKE_CURRENT_LIST_DIR}/die_scriptengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_scriptengine.h
    ${CMAKE_CURRENT_LIST_DIR}/global_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/global_script.h
    ${CMAKE_CURRENT_LIST_DIR}/xscriptengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xscriptengine.h
    ${CMAKE_CURRENT_LIST_DIR}/util_script.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util_script.h
)
