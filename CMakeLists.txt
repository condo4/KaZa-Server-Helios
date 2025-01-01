cmake_minimum_required(VERSION 3.16)

project(HeliosPlugin VERSION 0.1 LANGUAGES CXX)

option(BUILD_EXAMPLE "Build also example" OFF)

include(GNUInstallDirs)

if(NOT CMAKE_CROSSCOMPILING AND NOT DISABLE_PKG)
    if(EXISTS /etc/debian_version)
        message(STATUS "Build for debian package")
        set(BUILD_DEBIAN_PACKAGE ON)
    endif()
endif()


set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick SerialBus REQUIRED)
find_package(KaZa REQUIRED)
include_directories(${KAZA_INCLUDE_DIR})

add_library(HeliosPlugin SHARED
    src/helios.cpp src/helios.h
    src/plugin.cpp src/plugin.h
    qmldir
    KaZaHelios.qml
)

if(NOT DEFINED QML_MODULE_INSTALL_PATH)
    if(EXISTS /usr/lib64/qt6/qml/builtins.qmltypes)
        set(QML_MODULE_INSTALL_PATH "/usr/lib64/qt6/qml/")
    elseif(EXISTS /usr/lib/aarch64-linux-gnu/qt6/qml/builtins.qmltypes)
        set(QML_MODULE_INSTALL_PATH "/usr/lib/aarch64-linux-gnu/qt6/qml/")
    else()
        set(QML_MODULE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/qml )
    endif()
endif()

target_compile_definitions(HeliosPlugin PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(HeliosPlugin PRIVATE Qt6::Quick Qt6::SerialBus)
target_include_directories(HeliosPlugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(BUILD_EXAMPLE)
    # Example Project
    qt_add_executable(ExampleProject example/example.cpp)
    qt_add_qml_module(ExampleProject
        URI ExampleProjectApp
        VERSION 1.0
        QML_FILES example/example.qml example/HeliosControls.qml
    )
    target_link_libraries(ExampleProject PRIVATE Qt6::Quick)
    target_compile_definitions(ExampleProject PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
endif()

install(TARGETS HeliosPlugin DESTINATION ${QML_MODULE_INSTALL_PATH}/org/kazoe/helios)
install(FILES qmldir DESTINATION ${QML_MODULE_INSTALL_PATH}/org/kazoe/helios)
install(FILES KaZaHelios.qml DESTINATION ${QML_MODULE_INSTALL_PATH}/org/kazoe/helios)


if(BUILD_DEBIAN_PACKAGE)
    set(DEB_DEPEND "")
    file(READ /etc/debian_version DEBIAN_VERSION)
    string(REGEX MATCH "^[0-9]+" DEBIAN_MAJOR "${DEBIAN_VERSION}")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME "kazoe-helios")
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_PACKAGE_VERSION_PATCH "${CMAKE_PROJECT_VERSION_PATCH}-debian${DEBIAN_MAJOR}")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6serialbus6 qml6-module-qtqml")
    set(CPACK_PACKAGE_DESCRIPTION "Helios integration for QML")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Fabien Proriol <fabien.proriol@kazoe.org>")
    include(CPack)
endif()
