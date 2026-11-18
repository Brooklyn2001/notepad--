cmake_minimum_required(VERSION 3.16)
project(qscint CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT DEFINED QT_VERSION_MAJOR)
    set(QT_VERSION_MAJOR 5 CACHE STRING "Major version of Qt to use (5 or 6)")
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    if(NOT TARGET Qt6::Core)
        find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Concurrent Network Core5Compat)
    endif()
    set(QSCINT_QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport Qt6::Concurrent Qt6::Network Qt6::Core5Compat)
    set(QSCINT_SUFFIX "qt6")
    set(QSCINT_MIN_STD cxx_std_17)
else()
    if(NOT TARGET Qt5::Core)
        find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Concurrent Network)
    endif()
    set(QSCINT_QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::PrintSupport Qt5::Concurrent Qt5::Network)
    set(QSCINT_SUFFIX "qt5")
    set(QSCINT_MIN_STD cxx_std_11)
endif()

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/scintilla/lexers SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/scintilla/lexlib SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/scintilla/src SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/scintilla/boostregex SRC)
file(GLOB MOC_HEADER ${PROJECT_SOURCE_DIR}/src/Qsci/*.h)

add_library(${PROJECT_NAME} STATIC ${SRC} ${MOC_HEADER})

target_compile_definitions(${PROJECT_NAME} PRIVATE SCINTILLA_QT SCI_LEXER INCLUDE_DEPRECATED_FEATURES)
target_compile_features(${PROJECT_NAME} PRIVATE ${QSCINT_MIN_STD})

# Подавляем C4828 (BOM/спецсимволы в заголовках QScintilla)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4828)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/scintilla/lexlib
    ${PROJECT_SOURCE_DIR}/scintilla/boostregex)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/Qsci
    ${PROJECT_SOURCE_DIR}/scintilla/src
    ${PROJECT_SOURCE_DIR}/scintilla/include)

target_link_libraries(${PROJECT_NAME} ${QSCINT_QT_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME_DEBUG          "qmyedit_${QSCINT_SUFFIX}d"
    OUTPUT_NAME_RELWITHDEBINFO "qmyedit_${QSCINT_SUFFIX}d"
    OUTPUT_NAME_RELEASE        "qmyedit_${QSCINT_SUFFIX}"
    OUTPUT_NAME_MINSIZEREL     "qmyedit_${QSCINT_SUFFIX}")