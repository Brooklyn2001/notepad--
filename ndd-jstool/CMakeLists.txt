cmake_minimum_required(VERSION 3.16)
project(ndd-jstool)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- Выбор версии Qt (значение приходит из корневого CMakeLists.txt) ---
if(NOT DEFINED QT_VERSION_MAJOR)
    set(QT_VERSION_MAJOR 5 CACHE STRING "Major version of Qt to use (5 or 6)")
endif()

# --- Поиск Qt (если ещё не найден в корне) ---
if(QT_VERSION_MAJOR EQUAL 6)
    if(NOT TARGET Qt6::Core)
        find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Core5Compat)
    endif()
    set(NDD_JSTOOL_QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Core5Compat)
    set(NDD_JSTOOL_MIN_STD cxx_std_17)
else()
    if(NOT TARGET Qt5::Core)
        find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    endif()
    set(NDD_JSTOOL_QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
    set(NDD_JSTOOL_MIN_STD cxx_std_11)
endif()

# Путь к исходникам главного NDD-проекта (источник pluginGl.h и qscint)
if(NOT DEFINED NDD_SOURCE_DIR)
    set(NDD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

file(GLOB UI_SRC     ${PROJECT_SOURCE_DIR}/*.ui)
file(GLOB SRC        ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB MOC_HEADER ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED ${SRC} ${UI_SRC} ${MOC_HEADER})

# Стандарт C++ (Qt6 требует cxx_std_17, Qt5 — cxx_std_11)
target_compile_features(${PROJECT_NAME} PRIVATE ${NDD_JSTOOL_MIN_STD})

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

target_link_libraries(${PROJECT_NAME} PRIVATE qscint ${NDD_JSTOOL_QT_LIBS})

# Вывод плагина в каталог plugin/ рядом с исполняемым файлом
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin")
endif()