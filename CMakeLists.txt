project (stegano)

find_package(KDE4 REQUIRED)
find_package(QCA2 REQUIRED)

include_directories(${KDE4_INCLUDES})
 
if (CMAKE_COMPILER_IS_GNUCXX)
  #set ( CMAKE_CXX_FLAGS "-Wall -Weffc++ -ansi -pedantic")
  set ( CMAKE_CXX_FLAGS "-Wall -ansi -pedantic")
  #set ( CMAKE_CXX_FLAGS_DEBUG "/DEBUG_FLAGS_GO_HERE")
  #set ( CMAKE_CXX_FLAGS_RELEASE  "/RELEASE_FLAGS_GO_HERE" )
endif ()

add_subdirectory(app)
add_subdirectory(res)