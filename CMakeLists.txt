cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Project name
project("SkipIfZero Graphics")

# Sets build type to release if no build type is specified in a single-configuration generator.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Compiler flags
if(MSVC)
	# MSVC flags
	set(CMAKE_CXX_FLAGS "/Wall /Zi /EHsc /Gv /wd4201 /wd4350 /wd4514 /wd4571 /wd4668 /wd4710 /wd4711 /wd4820")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /fp:fast /DEBUG /DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /fp:fast /DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG  "/Od /DEBUG")
else()
	# GCC / Clang flags
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-c++11-extensions -std=c++11 -pthread")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -ffast-math -g -DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
endif()

# Directories
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(EXTERNALS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(CMAKE_MODULES ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Adds our cmake modules to module path.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULES})

# Finds SDL2 and SDL2_image, will make use of $SDL2 environment variable on Windows.
find_package(SDL2 REQUIRED) # ${SDL2_INCLUDE_DIR}, ${SDL2_LIBRARY}
find_package(SDL2_image REQUIRED) # ${SDL2_IMAGE_INCLUDE_DIR}, ${SDL2_IMAGE_LIBRARY}


# Subdirectories (libraries)
# SkipIfZeroCommon # ${SFZ_COMMON_INCLUDE_DIRS}, ${SFZ_COMMON_LIBRARIES}
add_subdirectory(${EXTERNALS_DIR}/SkipIfZeroCommon)

# GLEW
# find_package(GLEW REQUIRED) # ${GLEW_INCLUDE_DIRS}, ${GLEW_LIBRARIES}
add_definitions(-DGLEW_STATIC)
add_subdirectory(${EXTERNALS_DIR}/glew)

# Links OpenGL
find_package(OpenGL REQUIRED) # ${OPENGL_INCLUDE_DIR}, ${OPENGL_LIBRARIES}

# Include directories
include_directories(
	${INCLUDE_DIR}
	${SFZ_COMMON_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
)

# Sources and headers needed to compile library
set(SOURCE_BASE_FILES
	${INCLUDE_DIR}/sfz/GL.hpp
	${INCLUDE_DIR}/sfz/SDL.hpp)
source_group(sfz_graphics FILES ${SOURCE_BASE_FILES})

set(SOURCE_GL_FILES
	${INCLUDE_DIR}/sfz/gl/Alignment.hpp
	${INCLUDE_DIR}/sfz/gl/Context.hpp
	 ${SOURCE_DIR}/sfz/gl/Context.cpp
	${INCLUDE_DIR}/sfz/gl/OpenGL.hpp
	${INCLUDE_DIR}/sfz/gl/SpriteBatch.hpp
	 ${SOURCE_DIR}/sfz/gl/SpriteBatch.cpp
	${INCLUDE_DIR}/sfz/gl/Texture.hpp
	 ${SOURCE_DIR}/sfz/gl/Texture.cpp
	${INCLUDE_DIR}/sfz/gl/TexturePacker.hpp
	 ${SOURCE_DIR}/sfz/gl/TexturePacker.cpp
	${INCLUDE_DIR}/sfz/gl/TextureRegion.hpp
	${INCLUDE_DIR}/sfz/gl/Utils.hpp
	 ${SOURCE_DIR}/sfz/gl/Utils.cpp)
source_group(gl FILES ${SOURCE_GL_FILES})

set(SOURCE_SDL_FILES
	${INCLUDE_DIR}/sfz/sdl/GameController.hpp
	 ${SOURCE_DIR}/sfz/sdl/GameController.cpp
	${INCLUDE_DIR}/sfz/sdl/Session.hpp
	 ${SOURCE_DIR}/sfz/sdl/Session.cpp
	${INCLUDE_DIR}/sfz/sdl/Window.hpp
	 ${SOURCE_DIR}/sfz/sdl/Window.cpp)
source_group(sdl FILES ${SOURCE_SDL_FILES})

set(SOURCE_ALL_FILES
	${SOURCE_BASE_FILES}
	${SOURCE_GL_FILES}
	${SOURCE_SDL_FILES})

# Library compile target
add_library(SkipIfZeroGraphics ${SOURCE_ALL_FILES})

# Linking libraries
target_link_libraries(
	SkipIfZeroGraphics

	${SFZ_COMMON_LIBRARIES}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARY}
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
)

# Variables for linking library
set(SFZ_GRAPHICS_INCLUDE_DIRS
	${INCLUDE_DIR}
	${SFZ_COMMON_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	PARENT_SCOPE)
set(SFZ_GRAPHICS_LIBRARIES
	SkipIfZeroGraphics
	${SFZ_COMMON_LIBRARIES}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARY}
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
	PARENT_SCOPE)

# Test addding function
function(add_test_file test_name test_file_path)
	set(test_name_name ${test_name})
	add_executable(${test_name} ${test_file_path} ${SOURCE_ALL_FILES}) # Link with all files
	add_test(${test_name_name} ${test_name})
endfunction(add_test_file)

# Tests
if(SFZ_GRAPHICS_BUILD_TESTS)
	enable_testing(true)
endif()