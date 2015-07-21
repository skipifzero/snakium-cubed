cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project("snakium-cubed")

# Compiler flags
if(MSVC)
	# MSVC flags
	#set(cpp_compiler_flags "/Wall /EHsc /O2")
	set(cpp_compiler_flags "/O2")
else() # Assume we're compiling with CLANG or GCC if not MSVC.
	# GCC and Clang flags
	set(cpp_compiler_flags "-Wall -Wextra -Wno-c++11-extensions -std=c++11 -O3")
	set(c_compiler_flags "-Wall -Wextra -std=c11 -O3")
endif()

# Sets the specified compiler flags, will only set the first time cmake configures project.
if (NOT S3_CONFIGURED_ONCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cpp_compiler_flags}" 
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${c_compiler_flags}"
	    CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif ()

# Directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${SRC_DIR})
set(EXTERNALS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# Adds our cmake modules to module path.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# SkipIfZero Graphics
add_subdirectory(${EXTERNALS_DIR}/SkipIfZeroGraphics)

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem system)
link_directories(${Boost_LIBRARY_DIR})

# Include directories
include_directories(
	${INCLUDE_DIR}
	${SFZ_GRAPHICS_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# Source files
set(SOURCE_BASE_FILES
	${SRC_DIR}/Assets.hpp
	${SRC_DIR}/Assets.cpp
	${SRC_DIR}/Camera.hpp
	${SRC_DIR}/Camera.cpp
	${SRC_DIR}/GameLogic.hpp
	${SRC_DIR}/GlobalConfig.hpp
	${SRC_DIR}/GlobalConfig.cpp
	${SRC_DIR}/Main.cpp
	${SRC_DIR}/Rendering.hpp)
source_group(snakium_cubed_root FILES ${SOURCE_BASE_FILES})

set(SOURCE_GAMELOGIC_FILES
	${SRC_DIR}/gamelogic/Direction.hpp
	${SRC_DIR}/gamelogic/Direction.inl
	${SRC_DIR}/gamelogic/Model.hpp
	${SRC_DIR}/gamelogic/Model.cpp
	${SRC_DIR}/gamelogic/ModelConfig.hpp
	${SRC_DIR}/gamelogic/Position.hpp
	${SRC_DIR}/gamelogic/Position.inl
	${SRC_DIR}/gamelogic/SnakeTile.hpp
	${SRC_DIR}/gamelogic/SnakeTile.inl)
source_group(gamelogic FILES ${SOURCE_GAMELOGIC_FILES})

set(SOURCE_RENDERING_FILES
	${SRC_DIR}/rendering/S3Shaders.hpp
	${SRC_DIR}/rendering/S3Shaders.cpp
	${SRC_DIR}/rendering/TileObject.hpp
	${SRC_DIR}/rendering/TileObject.cpp)
source_group(rendering FILES ${SOURCE_RENDERING_FILES})

set(SOURCE_SCREENS_FILES
	${SRC_DIR}/screens/IScreen.hpp)
source_group(screens FILES ${SOURCE_SCREENS_FILES})

set(SOURCE_ALL_FILES
	${SOURCE_BASE_FILES}
	${SOURCE_GAMELOGIC_FILES}
	${SOURCE_RENDERING_FILES}
	${SOURCE_SCREENS_FILES})

# Main executable
add_executable(snakium-cubed ${SOURCE_ALL_FILES})

# Linking libraries to main executable
target_link_libraries(
	snakium-cubed

	${SFZ_GRAPHICS_LIBRARIES}
	${Boost_LIBRARIES}
)

# Specifies directory where generated binary and assets will be placed.
# Assets copying is currently only run when CMakeLists.txt is invoked.
message("Binary output directory: " ${CMAKE_BINARY_DIR}/bin)
INSTALL(TARGETS snakium-cubed DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Xcode specific file copying
if(CMAKE_GENERATOR STREQUAL Xcode)
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/Debug)
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/Release)
endif()

# MSVC specific file copying
if(MSVC)

	# Assets folder
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/Debug)
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/Release)

	# Runtime DLLs
	file(COPY RuntimeDLLs/msvc12_x64_debug/boost_filesystem-vc120-mt-gd-1_57.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/boost_system-vc120-mt-gd-1_57.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/libjpeg-9.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/libpng16-16.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/libtiff-5.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/libwebp-4.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/SDL2_image.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_debug/zlib1.dll DESTINATION ${CMAKE_BINARY_DIR})

	file(COPY RuntimeDLLs/msvc12_x64_release/boost_filesystem-vc120-mt-1_57.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/boost_system-vc120-mt-1_57.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/libjpeg-9.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/libpng16-16.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/libtiff-5.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/libwebp-4.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/SDL2_image.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/msvc12_x64_release/zlib1.dll DESTINATION ${CMAKE_BINARY_DIR})

endif()

# Sets flag that ensures some variables are only set once.
set(S3_CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once.")