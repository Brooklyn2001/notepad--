# cmake/QtVersionSetup.cmake
# Единая точка выбора Qt. Вызывается один раз в корневом CMakeLists.txt.

if(NOT DEFINED QT_VERSION_MAJOR)
    set(QT_VERSION_MAJOR 5 CACHE STRING "Major version of Qt to use (5 or 6)")
endif()
set_property(CACHE QT_VERSION_MAJOR PROPERTY STRINGS "5" "6")

if(QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS
        Core Gui Widgets Concurrent Network PrintSupport
        Xml Sql Core5Compat LinguistTools)
    set(NDD_QT_LIBS
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent
        Qt6::Network Qt6::PrintSupport Qt6::Xml Qt6::Sql Qt6::Core5Compat)
    set(NDD_QT_MIN_STD cxx_std_17)
else()
    find_package(Qt5 REQUIRED COMPONENTS
        Core Gui Widgets Concurrent Network PrintSupport
        XmlPatterns Sql LinguistTools)
    set(NDD_QT_LIBS
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent
        Qt5::Network Qt5::PrintSupport Qt5::XmlPatterns Qt5::Sql)
    set(NDD_QT_MIN_STD cxx_std_11)
endif()

# Фиксируем во всём дереве
set(QT_VERSION_MAJOR ${QT_VERSION_MAJOR} CACHE INTERNAL "" FORCE)