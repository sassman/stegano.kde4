include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BUILD_DIR}
    ${QT_INCLUDE_DIR}
    ${KDE4_INCLUDES}
)
include (KDE4Defaults)

set(stegano_SRCS
    main.cpp
    steganocore.cpp
    steganodialog.cpp
    bititerator.cpp
    messagecontainer.cpp
)

kde4_add_ui_files(stegano_SRCS ../res/ui/stegano.ui)
kde4_add_app_icon(stegano_SRCS ../res/icons/"hi*-apps-stegano.png")
kde4_add_executable(stegano ${stegano_SRCS})

target_link_libraries(stegano ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} qca plasma)

install( TARGETS stegano DESTINATION ${BIN_INSTALL_DIR})
install(FILES ../res/steganoui.rc
        DESTINATION  ${DATA_INSTALL_DIR}/stegano)

kde4_install_icons(${ICON_INSTALL_DIR})
kde4_update_iconcache()
