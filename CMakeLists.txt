cmake_minimum_required(VERSION 3.16)
project(NotePad-- VERSION 1.22.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent Network PrintSupport XmlPatterns)

# --- 生成语言翻译文件 .qm from .ts (与 realcompare_zh.qm 同理) ---
# 从已找到的 Qt5 安装目录推导 bin 路径，确保 find_program 能找到 lrelease
if(Qt5Core_DIR)
    get_filename_component(QT5_INSTALL_ROOT "${Qt5Core_DIR}/../../.." ABSOLUTE)
    set(LRELEASE_HINTS "${QT5_INSTALL_ROOT}/bin")
endif()
find_program(LRELEASE_EXECUTABLE NAMES lrelease lrelease-qt5 HINTS ${LRELEASE_HINTS})
if(LRELEASE_EXECUTABLE)
    file(GLOB APP_TS_FILES ${PROJECT_SOURCE_DIR}/src/*.ts)
    foreach(TS_FILE ${APP_TS_FILES})
        get_filename_component(TS_BASENAME ${TS_FILE} NAME_WE)
        set(QM_FILE ${PROJECT_SOURCE_DIR}/src/${TS_BASENAME}.qm)
        add_custom_command(
            OUTPUT ${QM_FILE}
            COMMAND ${LRELEASE_EXECUTABLE} ${TS_FILE} -qm ${QM_FILE}
            DEPENDS ${TS_FILE}
            COMMENT "lrelease: ${TS_BASENAME}.ts"
            VERBATIM)
        list(APPEND APP_QM_FILES ${QM_FILE})
    endforeach()
endif()

# qscint 关键依赖库
add_subdirectory(${PROJECT_SOURCE_DIR}/src/qscint)

# 插件库包含
# helloworld 动态插件库
add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin/helloworld)

# 社区插件 (ndd-jstool, ndd-xmltool)，作为动态插件库构建
if(NOT (PLUGIN_EN STREQUAL "on"))
    set(NDD_SOURCE_DIR ${PROJECT_SOURCE_DIR})
    add_subdirectory(${PROJECT_SOURCE_DIR}/ndd-jstool)
    add_subdirectory(${PROJECT_SOURCE_DIR}/ndd-xmltool)
    add_subdirectory(${PROJECT_SOURCE_DIR}/ndd-compareplus)
    add_subdirectory(${PROJECT_SOURCE_DIR}/ndd-edi)
endif()

# win下需要开启UNICODE进行支持TCHAR
if(CMAKE_HOST_WIN32)
    add_definitions(-D_UNICODE -DUNICODE)
endif()


if(${PLUGIN_EN})
    if(${PLUGIN_EN}  STREQUAL on)
        add_definitions(-DNO_PLUGIN=1)
    endif(${PLUGIN_EN})    
endif()



file(GLOB UI_SRC ${PROJECT_SOURCE_DIR}/src/*.ui)
set(UI_SRC ${UI_SRC} ${PROJECT_SOURCE_DIR}/src/cceditor/ccnotepad.ui)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/cceditor SRC)


if(CMAKE_HOST_WIN32)
# 添加 WIN32 保证主程序启动没有命令行
    list(APPEND WIN_RCS ${PROJECT_SOURCE_DIR}/src/RealCompareToMinGw.rc)
    add_executable(${PROJECT_NAME} WIN32 ${WIN_RCS} ${SRC} ${UI_SRC} ${APP_QM_FILES} ${PROJECT_SOURCE_DIR}/src/RealCompare.qrc)
else()
    add_executable(${PROJECT_NAME} ${SRC} ${UI_SRC} ${APP_QM_FILES} ${PROJECT_SOURCE_DIR}/src/RealCompare.qrc)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
${PROJECT_SOURCE_DIR}/src
${PROJECT_SOURCE_DIR}/src/cceditor

${PROJECT_SOURCE_DIR}/src/qscint/src
${PROJECT_SOURCE_DIR}/src/qscint/src/Qsci
${PROJECT_SOURCE_DIR}/src/qscint/scintilla/src
${PROJECT_SOURCE_DIR}/src/qscint/scintilla/include
${PROJECT_SOURCE_DIR}/src/qscint/scintilla/lexlib
${PROJECT_SOURCE_DIR}/src/qscint/scintilla/boostregex
)

target_link_libraries(${PROJECT_NAME} qscint Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent Qt5::Network  Qt5::PrintSupport Qt5::XmlPatterns)

# set(PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}/bin")
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if(CMAKE_HOST_UNIX)
    install(
        TARGETS ${PROJECT_NAME}
        DESTINATION "bin"
    )

    install(DIRECTORY  ${PROJECT_SOURCE_DIR}/src/linux/usr
            DESTINATION "/")

    include(${PROJECT_SOURCE_DIR}/cmake/deb_package_config.cmake) 
    include(CPack)
elseif(CMAKE_HOST_WIN32)
    install(TARGETS ${PROJECT_NAME}
            DESTINATION "/")

    install(DIRECTORY  ${PROJECT_SOURCE_DIR}/build/bin/
            DESTINATION "/")
    # 设置软件版本
    set(CPACK_PACKAGE_NAME "NotePad--")
    set(CPACK_PACKAGE_DESCRIPTION "NotePad--")
    set(CPACK_PACKAGE_COPYRIGHT "Copyright (c) 2023")
    set(CPACK_PACKAGE_VERSION "1.22.0")
    set(CPACK_PACKAGE_VERSION_MAJOR "1")
    set(CPACK_PACKAGE_VERSION_MINOR "22")
    set(CPACK_PACKAGE_VERSION_PATCH "0")

    include(${PROJECT_SOURCE_DIR}/cmake/nsis_package_config.cmake) 
    include(CPack)
endif()
