cmake_minimum_required(VERSION 2.8)
project( DOROTRON_002 )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# RAYLIB
add_subdirectory( ${PROJECT_SOURCE_DIR}/extdeps/raylib/ )
LINK_DIRECTORIES( ${PROJECT_BINARY_DIR}/extdeps/raylib/src/ )
include_directories( ${PROJECT_SOURCE_DIR}/extdeps/raylib/ )

# JSON
include_directories( ${PROJECT_SOURCE_DIR}/extdeps/json/single_include/ )

# THT SOURCES
file( GLOB_RECURSE THT_SRC ./src/*.cc )

add_executable( dorotron_002 ${THT_SRC} )
add_dependencies( dorotron_002 raylib )
target_link_libraries( dorotron_002 pthread raylib )
