set(kvkbd_SRCS vkeyboard.cpp
    x11keyboard.cpp
    kbdlayout.cpp
    dragwidget.cpp
    mainwidget.cpp
    vbutton.cpp
    main.cpp
    resizabledragwidget.cpp
    keysymconvert.cpp
    kbddock.cpp
    kvkbdapp.cpp
    kbdtray.cpp
    themeloader.cpp
)

SET(kvkbd_RESOURCES resources.qrc)

qt_add_resources(kvkbd_RESOURCES_RCC ${kvkbd_RESOURCES})

qt_add_dbus_adaptor(kvkbd_SRCS org.kde.kvkbd.Kvkbd.xml
                       mainwidget.h MainWidget)

qt_add_dbus_adaptor(kvkbd_SRCS org.kde.kvkbd.Dock.xml
                       kbddock.h KbdDock)

add_executable(kvkbd ${kvkbd_SRCS} ${kvkbd_RC_SRCS} ${kvkbd_RESOURCES_RCC})

target_link_libraries(kvkbd
                      Qt::Core
                      Qt::Gui
                      Qt::Xml
                      Qt::Widgets
                      Qt::DBus
                      KF${KF_VERSION}::ConfigCore
                      KF${KF_VERSION}::CoreAddons
                      KF${KF_VERSION}::I18n
                      KF${KF_VERSION}::XmlGui
                      KF${KF_VERSION}::WidgetsAddons
                      KF${KF_VERSION}::WindowSystem
                      X11
                      Xtst)

install(TARGETS kvkbd ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES kvkbd.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

add_subdirectory(colors)
add_subdirectory(themes)
