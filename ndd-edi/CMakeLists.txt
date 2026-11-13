cmake_minimum_required(VERSION 3.16)
project(ndd-edi)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt5 + QtSql (встроенный SQLite-драйвер QSQLITE — кросс-платформенно,
# внешняя System.Data.SQLite.dll не нужна).
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql)

# Путь к исходникам главного NDD-проекта (источник pluginGl.h и qscint)
if(NOT DEFINED NDD_SOURCE_DIR)
    set(NDD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

file(GLOB UI_SRC ${PROJECT_SOURCE_DIR}/*.ui)
file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB MOC_HEADER ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED ${SRC} ${UI_SRC} ${MOC_HEADER})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${NDD_SOURCE_DIR}/src/include
    ${NDD_SOURCE_DIR}/src/qscint/src
    ${NDD_SOURCE_DIR}/src/qscint/src/Qsci
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/src
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/include
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/lexlib
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/boostregex
)

target_link_libraries(${PROJECT_NAME} PRIVATE qscint Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql)

# Вывод плагина в каталог plugin/ рядом с исполняемым файлом
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin")
endif()
