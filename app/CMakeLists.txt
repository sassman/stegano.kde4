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
    MessageContainerFactory.cpp
)

kde4_add_ui_files(stegano_SRCS 
    ui/stegano.ui 
)

kde4_add_executable(stegano ${stegano_SRCS})

target_link_libraries(stegano ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} qca plasma quazip)

install( TARGETS stegano DESTINATION ${BIN_INSTALL_DIR})
