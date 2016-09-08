cmake_minimum_required(VERSION 2.8.12)

include(CMakeTools.txt)

include_directories(src)

TOOLS_DESCRIBE_EXTERNAL(X11 LINK_LIBRARIES "X11")
TOOLS_DESCRIBE_EXTERNAL(png LINK_LIBRARIES "png")
TOOLS_DESCRIBE_EXTERNAL(jpeg LINK_LIBRARIES "jpeg")

TOOLS_DESCRIBE_EXTERNAL(boost_system LINK_LIBRARIES "boost_system")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(POCKET_BUILD_TARGETS app common)

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
