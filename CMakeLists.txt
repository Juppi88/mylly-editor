####################################################################################################
# 
# This is the CMake file for Mylly editor tools collection.
# The recommended way to run this is in a separate build directory, e.g.
# 
# mkdir build && cd build
# cmake ..
# 
####################################################################################################

cmake_minimum_required(VERSION 3.0)
project(MyllyEditor VERSION 0.0.1 LANGUAGES CXX)

# Define compiler flags.
set(CMAKE_CXX_FLAGS         "-Wall -Wextra -std=c++11 -DCGLM_ALL_UNALIGNED")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -D_DEBUG -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")

# Add the CMake script for the engine library.
#add_subdirectory("../mylly/mylly" "mylly")

# Define source files for the game.
file(GLOB EDITOR_SRC
    "*.h"
    "*.cpp"
)

# Create a static library from the editor source.
add_library(mylly_editor ${EDITOR_SRC})

# TODO: Figure out how to safely remove the relative paths!
target_include_directories(mylly_editor
	PUBLIC	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE	${MYLLY_INCLUDE_DIRS} ../mylly ../mylly/mylly
)
