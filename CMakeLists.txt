cmake_minimum_required(VERSION 3.16)
project(NotePad-- VERSION 2.0.0)

option(PLUGIN_EN "Собрать без поддержки плагинов (определяет NO_PLUGIN=1)" OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# RPATH для Linux
if(CMAKE_HOST_UNIX AND NOT APPLE)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# --- Единая точка выбора Qt ---
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(QtVersionSetup)

# --- Генерация .qm из .ts ---
if(QT_VERSION_MAJOR EQUAL 6 AND TARGET Qt6::lrelease)
    get_target_property(LRELEASE_EXECUTABLE Qt6::lrelease IMPORTED_LOCATION)
endif()
if(NOT LRELEASE_EXECUTABLE)
    if(QT_VERSION_MAJOR EQUAL 6 AND Qt6Core_DIR)
        get_filename_component(QT_INSTALL_ROOT "${Qt6Core_DIR}/../../.." ABSOLUTE)
    elseif(Qt5Core_DIR)
        get_filename_component(QT_INSTALL_ROOT "${Qt5Core_DIR}/../../.." ABSOLUTE)
    endif()
    find_program(LRELEASE_EXECUTABLE
        NAMES lrelease lrelease-qt5 lrelease-qt6
        HINTS "${QT_INSTALL_ROOT}/bin")
endif()

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

# --- Подкаталоги ---
add_subdirectory(${PROJECT_SOURCE_DIR}/src/qscint)
#add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin/helloworld)

if(NOT PLUGIN_EN)
    set(NDD_SOURCE_DIR ${PROJECT_SOURCE_DIR})
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin/ndd-jstool)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin/ndd-xmltool)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin/ndd-compareplus)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin/ndd-edi)
endif()

if(PLUGIN_EN)
    add_definitions(-DNO_PLUGIN=1)
endif()

if(CMAKE_HOST_WIN32)
    add_definitions(-D_UNICODE -DUNICODE)
endif()

# --- Исходники ---
file(GLOB UI_SRC ${PROJECT_SOURCE_DIR}/src/*.ui)
list(APPEND UI_SRC ${PROJECT_SOURCE_DIR}/src/cceditor/ccnotepad.ui)

aux_source_directory(${PROJECT_SOURCE_DIR}/src         SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/cceditor SRC)

if(CMAKE_HOST_WIN32)
    list(APPEND WIN_RCS ${PROJECT_SOURCE_DIR}/src/RealCompareToMinGw.rc)
    add_executable(${PROJECT_NAME} WIN32
        ${WIN_RCS} ${SRC} ${UI_SRC} ${APP_QM_FILES}
        ${PROJECT_SOURCE_DIR}/src/RealCompare.qrc)
else()
    add_executable(${PROJECT_NAME}
        ${SRC} ${UI_SRC} ${APP_QM_FILES}
        ${PROJECT_SOURCE_DIR}/src/RealCompare.qrc)
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

target_compile_features(${PROJECT_NAME} PRIVATE ${NDD_QT_MIN_STD})
target_link_libraries(${PROJECT_NAME} PRIVATE qscint ${NDD_QT_LIBS})

# --- Установка и CPack ---
if(CMAKE_HOST_UNIX)
    install(TARGETS ${PROJECT_NAME} DESTINATION "bin")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/linux/usr DESTINATION "/")
    include(${PROJECT_SOURCE_DIR}/cmake/deb_package_config.cmake)
    include(CPack)
elseif(CMAKE_HOST_WIN32)
    install(TARGETS ${PROJECT_NAME} DESTINATION "/")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin/
        DESTINATION "/"
        PATTERN "Release" EXCLUDE)

    if(EXISTS "${PROJECT_SOURCE_DIR}/build/bin/plugin/Release")
        file(GLOB _plugin_dlls "${PROJECT_SOURCE_DIR}/build/bin/plugin/Release/*.dll")
        if(_plugin_dlls)
            install(FILES ${_plugin_dlls} DESTINATION "plugin")
        endif()
    endif()

    # windeployqt
    if(QT_VERSION_MAJOR EQUAL 6)
        if(TARGET Qt6::qmake)
            get_target_property(_qmake_exe Qt6::qmake IMPORTED_LOCATION)
        else()
            find_program(_qmake_exe NAMES qmake6 qmake-qt6 HINTS "${QT_INSTALL_ROOT}/bin")
        endif()
        if(_qmake_exe)
            get_filename_component(_qt_bin_dir "${_qmake_exe}" DIRECTORY)
            get_filename_component(_qt_root   "${_qt_bin_dir}/.." ABSOLUTE)
        endif()
        find_program(WINDEPLOYQT_EXECUTABLE
            NAMES windeployqt-qt6 windeployqt6 windeployqt
            HINTS "${_qt_bin_dir}")
    else()
        if(TARGET Qt5::qmake)
            get_target_property(_qmake_exe Qt5::qmake IMPORTED_LOCATION)
            get_filename_component(_qt_bin_dir "${_qmake_exe}" DIRECTORY)
            get_filename_component(_qt_root   "${_qt_bin_dir}/.." ABSOLUTE)
            set(WINDEPLOYQT_EXECUTABLE "${_qt_bin_dir}/windeployqt.exe")
        endif()
    endif()

    if(WINDEPLOYQT_EXECUTABLE AND EXISTS "${WINDEPLOYQT_EXECUTABLE}")
        install(CODE "
            message(STATUS \"Запуск windeployqt для \${CMAKE_INSTALL_PREFIX}/NotePad--.exe\")
            execute_process(
                COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                    --release --no-translations --no-compiler-runtime
                    --no-system-d3d-compiler --no-opengl-sw
                    \"\${CMAKE_INSTALL_PREFIX}/NotePad--.exe\"
                WORKING_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}\"
                RESULT_VARIABLE _wd_result)
            if(NOT _wd_result EQUAL 0)
                message(FATAL_ERROR \"windeployqt завершился с ошибкой: \${_wd_result}\")
            endif()
        ")
    else()
        message(WARNING "windeployqt не найден. Qt-зависимости не будут скопированы.")
    endif()

    # ndd-edi: QtSql + qsqlite
    if(QT_VERSION_MAJOR EQUAL 6)
        set(_SQL_DLL_NAME "Qt6Sql.dll")
    else()
        set(_SQL_DLL_NAME "Qt5Sql.dll")
    endif()
    set(_SQL_DRV_NAME "qsqlite.dll")

    if(EXISTS "${_qt_bin_dir}/${_SQL_DLL_NAME}")
        install(FILES "${_qt_bin_dir}/${_SQL_DLL_NAME}" DESTINATION "/")
    else()
        message(WARNING "${_SQL_DLL_NAME} не найден в ${_qt_bin_dir}")
    endif()

    if(EXISTS "${_qt_root}/plugins/sqldrivers/${_SQL_DRV_NAME}")
        install(FILES "${_qt_root}/plugins/sqldrivers/${_SQL_DRV_NAME}" DESTINATION "sqldrivers")
    else()
        message(WARNING "${_SQL_DRV_NAME} не найден в ${_qt_root}/plugins/sqldrivers")
    endif()

    if(EXISTS "${PROJECT_SOURCE_DIR}/ndd-edi/NppEdiAnalyzer.db")
        install(FILES "${PROJECT_SOURCE_DIR}/ndd-edi/NppEdiAnalyzer.db" DESTINATION "plugin")
    endif()

    set(CPACK_PACKAGE_NAME          "NotePad--")
    set(CPACK_PACKAGE_DESCRIPTION   "NotePad--")
    set(CPACK_PACKAGE_COPYRIGHT     "Copyright by brooklyn(c) 2026")
    set(CPACK_PACKAGE_VERSION       "2.0.0")
    set(CPACK_PACKAGE_VERSION_MAJOR "2")
    set(CPACK_PACKAGE_VERSION_MINOR "0")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
    include(${PROJECT_SOURCE_DIR}/cmake/nsis_package_config.cmake)
    include(CPack)
endif()