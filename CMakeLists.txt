cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project("snakium-cubed")

# Compiler flags
if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
  OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
  OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	# GCC and Clang flags
	set(compiler_flags "-Wall -Wextra -Wno-c++11-extensions -std=c++11 -O3")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# MSVC flags
	set(compiler_flags "/Wall /EHsc /O2")
else ()
	# Unknown compiler, sets no flags and prints warning.
	message("WARNING: Unknown compiler \"${CMAKE_CXX_COMPILER_ID}\", no specific flags set.")
endif ()

# Sets the specified compiler flags, will only set the first time cmake configures project.
if (NOT S3_CONFIGURED_ONCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_flags}" 
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_flags}"
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif ()


# Directories
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Links SDL2, will probably only work on unix-like systems with PkgConfig.
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)

# Links GLEW
include(FindGLEW)
find_package(GLEW REQUIRED)

# Include directories
include_directories(
	${SDL2_INCLUDE_DIRS}
	${SDL2IMAGE_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
)

# Main executable
add_executable(
	snakium-cubed
	
	${SOURCE_DIR}/Main.cpp
)

# Linking libraries to main executable
target_link_libraries(
	snakium-cubed

	${SDL2_LIBRARIES}
	${SDL2IMAGE_LIBRARIES}
	${GLEW_LIBRARIES}
)

# Sets flag that ensures some variables are only set once.
set(S3_CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once.")