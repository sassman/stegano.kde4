include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BUILD_DIR}
	${QT_INCLUDE_DIR}
	${KDE4_INCLUDES}
)
include (KDE4Defaults)

set(stegano.kde_SRCS
	main.cpp
	steganocore.cpp
	steganodialog.cpp
	bititerator.cpp
)

kde4_add_ui_files(stegano.kde_SRCS stegano.ui)
kde4_add_executable(stegano.kde ${stegano.kde_SRCS})

target_link_libraries(stegano.kde ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} qca plasma)