cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0063 NEW)

project(kvkbd)

option(ENABLE_QT6 "Compile with Qt6" ON)

if(ENABLE_QT6)
    set(QT_VERSION 6)
    set(KF_VERSION 6)
    set(REQUIRED_QT_VERSION 6.6.0)
    set(KF_MIN_VERSION 5.240.0)
else()
    set(QT_VERSION 5)
    set(KF_VERSION 5)
    set(REQUIRED_QT_VERSION 5.9.0)
    set(KF_MIN_VERSION 5.40.0)
    set(qt_add_resources qt5_add_resources)
    set(qt_add_dbus_adaptor qt5_add_dbus_adaptor)
endif()

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)

if(NOT XDG_APPS_INSTALL_DIR)
    set(XDG_APPS_INSTALL_DIR share/applications)
endif()

find_package(Qt${QT_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Gui
    Core
    Widgets
    Xml
    DBus)

find_package(KF${KF_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    Config
    CoreAddons
    XmlGui
    WindowSystem
    DocTools)

find_package(LibXslt REQUIRED)

add_subdirectory(doc)
add_subdirectory(src)
