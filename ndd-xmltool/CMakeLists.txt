cmake_minimum_required(VERSION 3.16)
project(ndd-xmltool)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

if(NOT DEFINED NDD_SOURCE_DIR)
    set(NDD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

# --- QuickXml ---
file(GLOB QUICKXML_SRC ${PROJECT_SOURCE_DIR}/QuickXmlLib/QuickXml/src/*.cpp)
add_library(QuickXml STATIC ${QUICKXML_SRC})
target_include_directories(QuickXml PUBLIC ${PROJECT_SOURCE_DIR}/QuickXmlLib/QuickXml/src)
target_compile_features(QuickXml PUBLIC cxx_std_17)
target_link_libraries(QuickXml PUBLIC Qt5::Core)

# --- SimpleXml ---
# Необходимы Qt (Q_OS_WIN/qglobal через qglobal.h) и C++17 (std::string_view)
file(GLOB SIMPLEXML_SRC ${PROJECT_SOURCE_DIR}/SimpleXmlLib/SimpleXml/src/*.cpp)
add_library(SimpleXml STATIC ${SIMPLEXML_SRC})
target_include_directories(SimpleXml PUBLIC
    ${PROJECT_SOURCE_DIR}/SimpleXmlLib/SimpleXml/src
    ${PROJECT_SOURCE_DIR}/SimpleXmlLib/SimpleXml/src/..)
target_compile_features(SimpleXml PUBLIC cxx_std_17)
target_link_libraries(SimpleXml PUBLIC Qt5::Core)

# --- StringXml (используется под #ifdef Q_OS_WIN; требует MSVC/ATL из-за Report.h) ---
if(MSVC)
    file(GLOB STRINGXML_SRC ${PROJECT_SOURCE_DIR}/StringXmlLib/StringXml/src/*.cpp)
    add_library(StringXml STATIC ${STRINGXML_SRC})
    target_include_directories(StringXml PUBLIC
        ${PROJECT_SOURCE_DIR}/StringXmlLib/StringXml/src
        ${PROJECT_SOURCE_DIR})
endif()

# --- Плагин ---
file(GLOB UI_SRC ${PROJECT_SOURCE_DIR}/*.ui)
file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB MOC_HEADER ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED ${SRC} ${UI_SRC} ${MOC_HEADER})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/QuickXmlLib/QuickXml/src
    ${PROJECT_SOURCE_DIR}/SimpleXmlLib/SimpleXml/src
    ${NDD_SOURCE_DIR}/src/include
    ${NDD_SOURCE_DIR}/src/qscint/src
    ${NDD_SOURCE_DIR}/src/qscint/src/Qsci
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/src
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/include
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/lexlib
    ${NDD_SOURCE_DIR}/src/qscint/scintilla/boostregex
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    QuickXml SimpleXml qscint Qt5::Core Qt5::Gui Qt5::Widgets)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE StringXml)
endif()

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin")
endif()
