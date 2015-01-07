cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Project name
project("SkipIfZero Common")

# Compiler flags
if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
  OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
  OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	# GCC and Clang flags
	set(compiler_flags "-Wall -Wextra -Wno-c++11-extensions -std=c++11 -pthread -O3")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# MSVC flags
	# set(compiler_flags "/Wall /EHsc /O2")
	set(compiler_flags "/O2")
else ()
	# Unknown compiler, sets no flags and prints warning.
	message("WARNING: Unknown compiler \"${CMAKE_CXX_COMPILER_ID}\", no specific flags set.")
endif ()

# Sets the specified compiler flags, will only set the first time cmake configures project.
if (NOT SFZC_CONFIGURED_ONCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_flags}" 
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_flags}"
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif ()

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
add_library(SkipIfZeroCommon
	${INCLUDE_DIR}/sfz/Math.hpp
	${INCLUDE_DIR}/sfz/math/MathConstants.hpp
	${INCLUDE_DIR}/sfz/math/Matrix.hpp
	${INCLUDE_DIR}/sfz/math/Matrix.inl
	${INCLUDE_DIR}/sfz/math/MatrixSupport.hpp
	${INCLUDE_DIR}/sfz/math/MatrixSupport.inl
	${INCLUDE_DIR}/sfz/math/Vector.hpp
	${INCLUDE_DIR}/sfz/math/Vector.inl
	${INCLUDE_DIR}/sfz/math/VectorSupport.hpp
	${INCLUDE_DIR}/sfz/math/VectorSupport.inl

	${INCLUDE_DIR}/sfz/Util.hpp
	${INCLUDE_DIR}/sfz/util/BlockingQueue.hpp
	${INCLUDE_DIR}/sfz/util/BlockingQueue.inl
	${INCLUDE_DIR}/sfz/util/ScopedThread.hpp
	 ${SOURCE_DIR}/sfz/util/ScopedThread.cpp
	${INCLUDE_DIR}/sfz/util/StopWatch.hpp
	 ${SOURCE_DIR}/sfz/util/StopWatch.cpp)

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

add_executable(BlockingQueue_Tests ${TEST_DIR}/sfz/util/BlockingQueue_Tests.cpp)
set_property(TARGET BlockingQueue_Tests APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
add_test(BlockingQueue_Tests_Name BlockingQueue_Tests)

# Sets flag that ensures some variables are only set once.
set(SFZC_CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once.")