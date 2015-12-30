cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project("SkipIfZero Common")

# Sets build type to release if no build type is specified in a single-configuration generator.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Compiler flags
if(MSVC)
	# MSVC flags
	set(CMAKE_CXX_FLAGS "/Wall /Zi /EHsc /wd4201 /wd4350 /wd4514 /wd4571 /wd4668 /wd4710 /wd4711 /wd4820")
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

# stb nothings
add_subdirectory(${EXTERNALS_DIR}/stb)

# tinyobjloader
add_subdirectory(${EXTERNALS_DIR}/tinyobjloader)

# GLEW
# find_package(GLEW REQUIRED) # ${GLEW_INCLUDE_DIRS}, ${GLEW_LIBRARIES}
add_definitions(-DGLEW_STATIC)
add_subdirectory(${EXTERNALS_DIR}/glew)

# Links OpenGL
find_package(OpenGL REQUIRED) # ${OPENGL_INCLUDE_DIR}, ${OPENGL_LIBRARIES}

# Catch
add_subdirectory(${EXTERNALS_DIR}/catch)

# Include directories
include_directories(
	${INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${STB_INCLUDE_DIRS}
	${TINYOBJLOADER_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${CATCH_INCLUDE_DIRS}
)

# Sources and headers needed to compile library
set(SOURCE_GENERAL_FILES
	${INCLUDE_DIR}/sfz/Assert.hpp
	${INCLUDE_DIR}/sfz/Assert.inl
	${INCLUDE_DIR}/sfz/PopWarnings.hpp
	${INCLUDE_DIR}/sfz/PushWarnings.hpp)
source_group(sfz FILES ${SOURCE_GENERAL_FILES})

set(SOURCE_GEOMETRY_FILES
	${INCLUDE_DIR}/sfz/Geometry.hpp
	${INCLUDE_DIR}/sfz/geometry/AABB.hpp
	${INCLUDE_DIR}/sfz/geometry/AABB.inl
	${INCLUDE_DIR}/sfz/geometry/AABB2D.hpp
	${INCLUDE_DIR}/sfz/geometry/AABB2D.inl
	${INCLUDE_DIR}/sfz/geometry/Circle.hpp
	${INCLUDE_DIR}/sfz/geometry/Circle.inl
	${INCLUDE_DIR}/sfz/geometry/Intersection.hpp
	 ${SOURCE_DIR}/sfz/geometry/Intersection.cpp
	${INCLUDE_DIR}/sfz/geometry/OBB.hpp
	${INCLUDE_DIR}/sfz/geometry/OBB.inl
	${INCLUDE_DIR}/sfz/geometry/Plane.hpp
	${INCLUDE_DIR}/sfz/geometry/Plane.inl
	${INCLUDE_DIR}/sfz/geometry/Sphere.hpp
	${INCLUDE_DIR}/sfz/geometry/Sphere.inl
	${INCLUDE_DIR}/sfz/geometry/ViewFrustum.hpp
	 ${SOURCE_DIR}/sfz/geometry/ViewFrustum.cpp)
source_group(sfz_geometry FILES ${SOURCE_GEOMETRY_FILES})

set(SOURCE_GL_FILES
	${INCLUDE_DIR}/sfz/GL.hpp
	${INCLUDE_DIR}/sfz/gl/Alignment.hpp
	${INCLUDE_DIR}/sfz/gl/Context.hpp
	 ${SOURCE_DIR}/sfz/gl/Context.cpp
	${INCLUDE_DIR}/sfz/gl/FontRenderer.hpp
	 ${SOURCE_DIR}/sfz/gl/FontRenderer.cpp
	${INCLUDE_DIR}/sfz/gl/FrameBuffer.hpp
	 ${SOURCE_DIR}/sfz/gl/Framebuffer.cpp
	${INCLUDE_DIR}/sfz/gl/GLUtils.hpp
	 ${SOURCE_DIR}/sfz/gl/GLUtils.cpp
	${INCLUDE_DIR}/sfz/gl/OpenGL.hpp
	${INCLUDE_DIR}/sfz/gl/PostProcessQuad.hpp
	 ${SOURCE_DIR}/sfz/gl/PostProcessQuad.cpp
	${INCLUDE_DIR}/sfz/gl/Program.hpp
	 ${SOURCE_DIR}/sfz/gl/Program.cpp
	${INCLUDE_DIR}/sfz/gl/SimpleModel.hpp
	 ${SOURCE_DIR}/sfz/gl/SimpleModel.cpp
	${INCLUDE_DIR}/sfz/gl/Spotlight.hpp
	 ${SOURCE_DIR}/sfz/gl/Spotlight.cpp
	${INCLUDE_DIR}/sfz/gl/SpriteBatch.hpp
	 ${SOURCE_DIR}/sfz/gl/SpriteBatch.cpp
	${INCLUDE_DIR}/sfz/gl/Texture.hpp
	 ${SOURCE_DIR}/sfz/gl/Texture.cpp
	${INCLUDE_DIR}/sfz/gl/TextureEnums.hpp
	${INCLUDE_DIR}/sfz/gl/TexturePacker.hpp
	 ${SOURCE_DIR}/sfz/gl/TexturePacker.cpp
	${INCLUDE_DIR}/sfz/gl/TextureRegion.hpp
	${INCLUDE_DIR}/sfz/gl/ViewFrustumMesh.hpp
	 ${SOURCE_DIR}/sfz/gl/ViewFrustumMesh.cpp)
source_group(sfz_gl FILES ${SOURCE_GL_FILES})

set(SOURCE_MATH_FILES
	${INCLUDE_DIR}/sfz/Math.hpp
	${INCLUDE_DIR}/sfz/math/MathConstants.hpp
	${INCLUDE_DIR}/sfz/math/MathHelpers.hpp
	${INCLUDE_DIR}/sfz/math/MathHelpers.inl
	${INCLUDE_DIR}/sfz/math/Matrix.hpp
	${INCLUDE_DIR}/sfz/math/Matrix.inl
	${INCLUDE_DIR}/sfz/math/MatrixSupport.hpp
	${INCLUDE_DIR}/sfz/math/MatrixSupport.inl
	${INCLUDE_DIR}/sfz/math/Vector.hpp
	${INCLUDE_DIR}/sfz/math/Vector.inl)
source_group(sfz_math FILES ${SOURCE_MATH_FILES})

set(SOURCE_SDL_FILES
	${INCLUDE_DIR}/sfz/SDL.hpp
	${INCLUDE_DIR}/sfz/sdl/ButtonState.hpp
	${INCLUDE_DIR}/sfz/sdl/GameController.hpp
	 ${SOURCE_DIR}/sfz/sdl/GameController.cpp
	${INCLUDE_DIR}/sfz/sdl/Mouse.hpp
	 ${SOURCE_DIR}/sfz/sdl/Mouse.cpp
	${INCLUDE_DIR}/sfz/sdl/Session.hpp
	 ${SOURCE_DIR}/sfz/sdl/Session.cpp
	${INCLUDE_DIR}/sfz/sdl/Window.hpp
	 ${SOURCE_DIR}/sfz/sdl/Window.cpp)
source_group(sfz_sdl FILES ${SOURCE_SDL_FILES})

set(SOURCE_SCREENS_FILES
	${INCLUDE_DIR}/sfz/Screens.hpp
	${INCLUDE_DIR}/sfz/screens/BaseScreen.hpp
	${INCLUDE_DIR}/sfz/screens/GameLoop.hpp
	 ${SOURCE_DIR}/sfz/screens/GameLoop.cpp)
source_group(sfz_screens FILES ${SOURCE_SCREENS_FILES})

set(SOURCE_UTIL_FILES
	${INCLUDE_DIR}/sfz/Util.hpp
	${INCLUDE_DIR}/sfz/util/FrametimeStats.hpp
	 ${SOURCE_DIR}/sfz/util/FrametimeStats.cpp
	${INCLUDE_DIR}/sfz/util/IniParser.hpp
	 ${SOURCE_DIR}/sfz/util/IniParser.cpp
	${INCLUDE_DIR}/sfz/util/IO.hpp
	 ${SOURCE_DIR}/sfz/util/IO.cpp
	${INCLUDE_DIR}/sfz/util/StopWatch.hpp
	 ${SOURCE_DIR}/sfz/util/StopWatch.cpp)
source_group(sfz_util FILES ${SOURCE_UTIL_FILES})

set(SOURCE_ALL_FILES
	${SOURCE_GENERAL_FILES}
	${SOURCE_GEOMETRY_FILES}
	${SOURCE_GL_FILES}
	${SOURCE_MATH_FILES}
	${SOURCE_SDL_FILES}
	${SOURCE_SCREENS_FILES}
	${SOURCE_UTIL_FILES})

# Library compile target
add_library(SkipIfZeroCommon ${SOURCE_ALL_FILES})

# Linking libraries
target_link_libraries(
	SkipIfZeroCommon

	${SDL2_LIBRARY}
	${TINYOBJLOADER_LIBRARIES}
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
)

# Variables for linking library
set(SFZ_COMMON_INCLUDE_DIRS
	${INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${TINYOBJLOADER_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	PARENT_SCOPE)
set(SFZ_COMMON_LIBRARIES
	SkipIfZeroCommon
	${SDL2_LIBRARY}
	${TINYOBJLOADER_LIBRARIES}
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
	PARENT_SCOPE)

# Test addding function
function(add_test_file test_name test_files)
	set(test_name_name ${test_name})
	add_executable(${test_name} ${test_files})
	target_link_libraries(
		${test_name}
		
		SkipIfZeroCommon
		${SDL2_LIBRARY}
		${GLEW_LIBRARIES}
		${OPENGL_LIBRARIES}
	)
	add_test(${test_name_name} ${test_name})
endfunction(add_test_file)

# Tests
if(SFZ_COMMON_BUILD_TESTS)
	enable_testing(true)
	add_test_file(Intersection_Tests ${TEST_DIR}/sfz/geometry/Intersection_Tests.cpp)
	add_test_file(IO_Tests ${TEST_DIR}/sfz/util/IO_Tests.cpp)
	add_test_file(MathConstants_Tests ${TEST_DIR}/sfz/math/MathConstants_Tests.cpp)
	add_test_file(Matrix_Tests ${TEST_DIR}/sfz/math/Matrix_Tests.cpp)
	add_test_file(Vector_Tests ${TEST_DIR}/sfz/math/Vector_Tests.cpp)
	
endif()