cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project("snakium-cubed")

# Sets build type to release if no build type is specified in a single-configuration generator.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Compiler flags
if(MSVC)
	# MSVC flags
	set(CMAKE_CXX_FLAGS "/Wall /Zi /EHsc /wd4514 /wd4668 /wd4710 /wd4711 /wd4820")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /DEBUG /DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG  "/Od /DEBUG")
else()
	# GCC / Clang flags
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-c++11-extensions -std=c++11 -pthread")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -ffast-math -g -DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -DSFZ_NO_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
endif()

# Directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${SRC_DIR})
set(EXTERNALS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# SkipIfZero Graphics
add_subdirectory(${EXTERNALS_DIR}/SkipIfZeroGraphics)

# Include directories
include_directories(
	${INCLUDE_DIR}
	${SFZ_GRAPHICS_INCLUDE_DIRS}
)

# Source files
set(SOURCE_BASE_FILES
	${SRC_DIR}/Assets.hpp
	${SRC_DIR}/Assets.cpp
	${SRC_DIR}/Button.hpp
	${SRC_DIR}/Button.cpp
	${SRC_DIR}/Camera.hpp
	${SRC_DIR}/Camera.cpp
	${SRC_DIR}/GameLogic.hpp
	${SRC_DIR}/GlobalConfig.hpp
	${SRC_DIR}/GlobalConfig.cpp
	${SRC_DIR}/Main.cpp
	${SRC_DIR}/Rendering.hpp
	${SRC_DIR}/Screens.hpp)
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
	${SRC_DIR}/screens/GameScreen.hpp
	${SRC_DIR}/screens/GameScreen.cpp
	${SRC_DIR}/screens/MainMenuScreen.hpp
	${SRC_DIR}/screens/MainMenuScreen.cpp
	${SRC_DIR}/screens/ScreenMenuConstants.hpp)
source_group(screens FILES ${SOURCE_SCREENS_FILES})

set(SOURCE_SFZ_TEMP_FILES
	${SRC_DIR}/sfz/util/IniParser.hpp
	${SRC_DIR}/sfz/util/IniParser.cpp
	${SRC_DIR}/sfz/geometry/Circle.hpp
	${SRC_DIR}/sfz/geometry/Circle.inl
	${SRC_DIR}/sfz/geometry/Rectangle.hpp
	${SRC_DIR}/sfz/geometry/Rectangle.inl)
source_group(sfz_temp FILES ${SOURCE_SFZ_TEMP_FILES})

set(SOURCE_ALL_FILES
	${SOURCE_BASE_FILES}
	${SOURCE_GAMELOGIC_FILES}
	${SOURCE_RENDERING_FILES}
	${SOURCE_SCREENS_FILES}
	${SOURCE_SFZ_TEMP_FILES})

# Main executable
add_executable(snakium-cubed ${SOURCE_ALL_FILES})

# Linking libraries to main executable
target_link_libraries(
	snakium-cubed

	${SFZ_GRAPHICS_LIBRARIES}
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
	file(COPY RuntimeDLLs/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY RuntimeDLLs/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug)
	file(COPY RuntimeDLLs/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR}/Release)
endif()