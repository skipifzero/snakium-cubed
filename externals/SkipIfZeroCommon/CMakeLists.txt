cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Project name
project("SkipIfZero Common")

# Sets build type to release if no build type is specified in a single-configuration generator.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Compiler flags
if(MSVC)
	# MSVC flags
	set(CMAKE_CXX_FLAGS "/Wall /EHsc /wd4350 /wd4514 /wd4668 /wd4710 /wd4711 /wd4820")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG  "/Od")
else()
	# GCC / Clang flags
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-c++11-extensions -std=c++11 -pthread")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
endif()

# Directories
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(EXTERNALS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# Subdirectories (libraries)
add_subdirectory(${EXTERNALS_DIR}/catch)

# Include directories
include_directories(${INCLUDE_DIR})
include_directories(${CATCH_INCLUDE_DIRS})

# Sources and headers needed to compile library
set(SOURCE_GENERAL_FILES
	${INCLUDE_DIR}/sfz/Assert.hpp
	${INCLUDE_DIR}/sfz/Assert.inl
	${INCLUDE_DIR}/sfz/MSVC12HackON.hpp
	${INCLUDE_DIR}/sfz/MSVC12HackOFF.hpp
	${INCLUDE_DIR}/sfz/Geometry.hpp
	${INCLUDE_DIR}/sfz/Math.hpp
	${INCLUDE_DIR}/sfz/Util.hpp)
source_group(sfz FILES ${SOURCE_GENERAL_FILES})

set(SOURCE_GEOMETRY_FILES
	${INCLUDE_DIR}/sfz/geometry/AABB.hpp
	${INCLUDE_DIR}/sfz/geometry/AABB.inl
	${INCLUDE_DIR}/sfz/geometry/Intersection.hpp
	 ${SOURCE_DIR}/sfz/geometry/Intersection.cpp
	${INCLUDE_DIR}/sfz/geometry/OBB.hpp
	${INCLUDE_DIR}/sfz/geometry/OBB.inl
	${INCLUDE_DIR}/sfz/geometry/Plane.hpp
	${INCLUDE_DIR}/sfz/geometry/Plane.inl
	${INCLUDE_DIR}/sfz/geometry/Sphere.hpp
	${INCLUDE_DIR}/sfz/geometry/Sphere.inl)
source_group(sfz_geometry FILES ${SOURCE_GEOMETRY_FILES})

set(SOURCE_MATH_FILES
	${INCLUDE_DIR}/sfz/math/MathConstants.hpp
	${INCLUDE_DIR}/sfz/math/MathHelpers.hpp
	${INCLUDE_DIR}/sfz/math/MathHelpers.inl
	${INCLUDE_DIR}/sfz/math/Matrix.hpp
	${INCLUDE_DIR}/sfz/math/Matrix.inl
	${INCLUDE_DIR}/sfz/math/MatrixSupport.hpp
	${INCLUDE_DIR}/sfz/math/MatrixSupport.inl
	${INCLUDE_DIR}/sfz/math/Vector.hpp
	${INCLUDE_DIR}/sfz/math/Vector.inl
	${INCLUDE_DIR}/sfz/math/VectorSupport.hpp
	${INCLUDE_DIR}/sfz/math/VectorSupport.inl)
source_group(sfz_math FILES ${SOURCE_MATH_FILES})

set(SOURCE_UTIL_FILES
	${INCLUDE_DIR}/sfz/util/BlockingQueue.hpp
	${INCLUDE_DIR}/sfz/util/BlockingQueue.inl
	${INCLUDE_DIR}/sfz/util/ScopedThread.hpp
	 ${SOURCE_DIR}/sfz/util/ScopedThread.cpp
	${INCLUDE_DIR}/sfz/util/StopWatch.hpp
	 ${SOURCE_DIR}/sfz/util/StopWatch.cpp)
source_group(sfz_util FILES ${SOURCE_UTIL_FILES})

set(SOURCE_ALL_FILES
	${SOURCE_GENERAL_FILES}
	${SOURCE_GEOMETRY_FILES}
	${SOURCE_MATH_FILES}
	${SOURCE_UTIL_FILES})

# Library compile target
add_library(SkipIfZeroCommon ${SOURCE_ALL_FILES})

# Variables for linking library
set(SFZ_COMMON_INCLUDE_DIRS ${INCLUDE_DIR} PARENT_SCOPE)
set(SFZ_COMMON_LIBRARIES SkipIfZeroCommon PARENT_SCOPE)

# Enables testing
enable_testing(true)

# Tests
add_executable(Vector_Tests ${TEST_DIR}/sfz/math/Vector_Tests.cpp)
set_property(TARGET Vector_Tests APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
add_test(Vector_Tests_Name Vector_Tests)

add_executable(Matrix_Tests ${TEST_DIR}/sfz/math/Matrix_Tests.cpp)
set_property(TARGET Matrix_Tests APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
add_test(Matrix_Tests_Name Matrix_Tests)

add_executable(MathConstants_Tests ${TEST_DIR}/sfz/math/MathConstants_Tests.cpp)
set_property(TARGET MathConstants_Tests APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
add_test(MathConstants_Tests_Name MathConstants_Tests)

add_executable(Intersection_Tests ${TEST_DIR}/sfz/geometry/Intersection_Tests.cpp ${SOURCE_ALL_FILES})
set_property(TARGET Intersection_Tests APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
add_test(Intersection_Tests_Name Intersection_Tests)

add_executable(BlockingQueue_Tests ${TEST_DIR}/sfz/util/BlockingQueue_Tests.cpp)
set_property(TARGET BlockingQueue_Tests APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
add_test(BlockingQueue_Tests_Name BlockingQueue_Tests)