cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project("snakium-cubed")

# Compiler flags
if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
  OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
  OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	# GCC and Clang flags
	set(cpp_compiler_flags "-Wall -Wextra -Wno-c++11-extensions -std=c++11 -O3")
	set(c_compiler_flags "-Wall -Wextra -std=c11 -O3")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# MSVC flags
	#set(cpp_compiler_flags "/Wall /EHsc /O2")
	set(cpp_compiler_flags "/O2")
else ()
	# Unknown compiler, sets no flags and prints warning.
	message("WARNING: Unknown compiler \"${CMAKE_CXX_COMPILER_ID}\", no specific flags set.")
endif ()

# Sets the specified compiler flags, will only set the first time cmake configures project.
if (NOT S3_CONFIGURED_ONCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cpp_compiler_flags}" 
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${c_compiler_flags}"
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif ()

# Directories
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src) # Same as source dir since no library.
set(EXTERNALS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# Adds our cmake modules to module path.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# SkipIfZero Common
add_subdirectory(${EXTERNALS_DIR}/SkipIfZeroCommon)

# Finds SDL2 and SDL2_image, will apparently make use of $SDL2 environment variable on Windows.
find_package(SDL2 REQUIRED) # ${SDL2_INCLUDE_DIR}, ${SDL2_LIBRARY}
find_package(SDL2_image REQUIRED) # ${SDL2_IMAGE_INCLUDE_DIR}, ${SDL2_IMAGE_LIBRARY}

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

# Main executable
add_executable(
	snakium-cubed
	
	 ${SOURCE_DIR}/Main.cpp
	${INCLUDE_DIR}/Camera.hpp
	 ${SOURCE_DIR}/Camera.cpp
	${INCLUDE_DIR}/SnakiumCubedShaders.hpp
	 ${SOURCE_DIR}/SnakiumCubedShaders.cpp
	${INCLUDE_DIR}/TileObject.hpp
	 ${SOURCE_DIR}/TileObject.cpp
	${INCLUDE_DIR}/Assets.hpp
	 ${SOURCE_DIR}/Assets.cpp
	
	${INCLUDE_DIR}/GameLogic.hpp
	${INCLUDE_DIR}/gamelogic/Config.hpp
	${INCLUDE_DIR}/gamelogic/Direction.hpp
	${INCLUDE_DIR}/gamelogic/Direction.inl
	${INCLUDE_DIR}/gamelogic/Position.hpp
	${INCLUDE_DIR}/gamelogic/Position.inl
	${INCLUDE_DIR}/gamelogic/Model.hpp
	 ${SOURCE_DIR}/gamelogic/Model.cpp
	${INCLUDE_DIR}/gamelogic/SnakeTile.hpp
	${INCLUDE_DIR}/gamelogic/SnakeTile.inl

	${INCLUDE_DIR}/sfz/SDL.hpp
	${INCLUDE_DIR}/sfz/sdl/Session.hpp
	 ${SOURCE_DIR}/sfz/sdl/Session.cpp
	${INCLUDE_DIR}/sfz/sdl/Window.hpp
	 ${SOURCE_DIR}/sfz/sdl/Window.cpp

	${INCLUDE_DIR}/sfz/GL.hpp
	${INCLUDE_DIR}/sfz/gl/Context.hpp
	 ${SOURCE_DIR}/sfz/gl/Context.cpp
	${INCLUDE_DIR}/sfz/gl/Texture.hpp
	 ${SOURCE_DIR}/sfz/gl/Texture.cpp
	${INCLUDE_DIR}/sfz/gl/Utils.hpp
	 ${SOURCE_DIR}/sfz/gl/Utils.cpp
)

# Linking libraries to main executable
target_link_libraries(
	snakium-cubed

	${SFZ_COMMON_LIBRARIES}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARY}
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
)

# Specifies directory where generated binary and assets will be placed.
# Assets copying is currently only run when CMakeLists.txt is invoked.
message("Binary output directory: " ${CMAKE_BINARY_DIR}/bin)
INSTALL(TARGETS snakium-cubed DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Sets flag that ensures some variables are only set once.
set(S3_CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once.")