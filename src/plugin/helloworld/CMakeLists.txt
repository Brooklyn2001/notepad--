cmake_minimum_required(VERSION 3.16)
project(helloworld)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- Выбор версии Qt (значение приходит из корневого CMakeLists.txt) ---
if(NOT DEFINED QT_VERSION_MAJOR)
    set(QT_VERSION_MAJOR 5 CACHE STRING "Major version of Qt to use (5 or 6)")
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    # XmlPatterns в Qt6 удалён -> Xml + Core5Compat
    find_package(Qt6 REQUIRED COMPONENTS
        Core Gui Widgets Concurrent Network PrintSupport Xml Core5Compat)
    set(HELLOWORLD_QT_LIBS
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent
        Qt6::Network Qt6::PrintSupport Qt6::Xml Qt6::Core5Compat)
else()
    find_package(Qt5 REQUIRED COMPONENTS
        Core Gui Widgets Concurrent Network PrintSupport XmlPatterns)
    set(HELLOWORLD_QT_LIBS
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent
        Qt5::Network Qt5::PrintSupport Qt5::XmlPatterns)
endif()

# UNICODE нужен только под Windows (TCHAR)
if(CMAKE_HOST_WIN32)
    add_definitions(-D_UNICODE -DUNICODE)
endif()

file(GLOB UI_SRC     ${PROJECT_SOURCE_DIR}/*.ui)
file(GLOB SRC        ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB MOC_HEADER ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED ${SRC} ${UI_SRC} ${MOC_HEADER})

# Qt6 требует C++17, Qt5 достаточно C++11
if(QT_VERSION_MAJOR EQUAL 6)
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
else()
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugin")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/../../include
    ${PROJECT_SOURCE_DIR}/../../qscint/src
    ${PROJECT_SOURCE_DIR}/../../qscint/src/Qsci
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/src
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/include
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/lexlib
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/boostregex
)

target_link_libraries(${PROJECT_NAME} qscint ${HELLOWORLD_QT_LIBS})