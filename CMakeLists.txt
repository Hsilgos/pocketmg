cmake_minimum_required(VERSION 2.8.12)

include(CMakeTools.txt)

include_directories(src)

TOOLS_DESCRIBE_EXTERNAL(X11 LINK_LIBRARIES "X11")
TOOLS_DESCRIBE_EXTERNAL(png LINK_LIBRARIES "png")
TOOLS_DESCRIBE_EXTERNAL(jpeg LINK_LIBRARIES "jpeg")
TOOLS_DESCRIBE_EXTERNAL(zip LINK_LIBRARIES "z")
TOOLS_DESCRIBE_EXTERNAL(dl LINK_LIBRARIES "dl")

find_package(Boost 1.54 REQUIRED COMPONENTS system)

TOOLS_DESCRIBE_EXTERNAL(boost_system INCLUDE_DIRS ${Boost_INCLUDE_DIRS} LINK_LIBRARIES ${Boost_SYSTEM_LIBRARY})
TOOLS_DESCRIBE_EXTERNAL(pthread LINK_LIBRARIES "pthread")

#TOOLS_DESCRIBE_EXTERNAL(seven_zip
#    INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/p7zip/C" "${CMAKE_CURRENT_SOURCE_DIR}/p2zip/CPP"
#    LINK_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/p7zip/bin/7z.so")

#add_library(7z SHARED IMPORTED )
#set_target_properties(7z PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/p7zip/bin/7z.so")

#TOOLS_DESCRIBE_EXTERNAL(seven_zip
#    INCLUDE_DIRS
#        "${CMAKE_CURRENT_SOURCE_DIR}/p7zip/C"
#        "${CMAKE_CURRENT_SOURCE_DIR}/p7zip/CPP"
#        "${CMAKE_CURRENT_SOURCE_DIR}/p7zip/CPP/include_windows"
#    LINK_LIBRARIES
#        7z)

# 7Z
set (SEVEN_ZIP_STATIC "${CMAKE_CURRENT_SOURCE_DIR}/p7zip")
include_directories(
  "${SEVEN_ZIP_STATIC}/CPP/myWindows"
  "${SEVEN_ZIP_STATIC}/CPP"
  "${SEVEN_ZIP_STATIC}/CPP/include_windows"
)
#add_definitions( -DEXTERNAL_CODECS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_REENTRANT -DENV_UNIX -DBREAK_HANDLER -DUNICODE -D_UNICODE -DUNIX_USE_WIN_FILE)
add_definitions(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_REENTRANT -DENV_UNIX -DBREAK_HANDLER -DUNICODE -D_UNICODE -DUNIX_USE_WIN_FILE)
#~

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")

set(POCKET_BUILD_TARGETS app common 7z_static)
add_definitions(-DNOUNCRYPT)

# Projects
foreach(SUBPROJ ${POCKET_BUILD_TARGETS})
    add_subdirectory("./src/${SUBPROJ}")

#    set(TESTSUBPROJ "${SUBPROJ}_test")
#    if(EXISTS "${CMAKE_SOURCE_DIR}/Src/Tests/${TESTSUBPROJ}")
#        add_subdirectory("${CMAKE_SOURCE_DIR}/Src/Tests/${TESTSUBPROJ}")
#    endif()
endforeach()


include_directories(src/common)
add_subdirectory("./test")
