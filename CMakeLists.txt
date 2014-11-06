cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project("snakium-cubed")

# Directories
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Links SDL2, will probably only work on unix-like systems with PkgConfig.
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED sdl2_image)

# Include directories
include_directories(
	${SDL2_INCLUDE_DIRS}
	${SDL2IMAGE_INCLUDE_DIRS}
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
)