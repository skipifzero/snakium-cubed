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

# SkipIfZero Common
add_subdirectory(${EXTERNALS_DIR}/SkipIfZeroCommon)

# Include directories
include_directories(
	${INCLUDE_DIR}
	${SFZ_COMMON_INCLUDE_DIRS}
)

# Source files
set(SOURCE_BASE_FILES
	${SRC_DIR}/GlobalConfig.hpp
	${SRC_DIR}/GlobalConfig.cpp
	${SRC_DIR}/Main.cpp)
source_group(snakium_cubed_root FILES ${SOURCE_BASE_FILES})

set(SOURCE_GAMELOGIC_FILES
	${SRC_DIR}/GameLogic.hpp
	${SRC_DIR}/gamelogic/Direction.hpp
	${SRC_DIR}/gamelogic/Direction.cpp
	${SRC_DIR}/gamelogic/Model.hpp
	${SRC_DIR}/gamelogic/Model.cpp
	${SRC_DIR}/gamelogic/ModelConfig.hpp
	${SRC_DIR}/gamelogic/Position.hpp
	${SRC_DIR}/gamelogic/Position.cpp
	${SRC_DIR}/gamelogic/SnakeTile.hpp
	${SRC_DIR}/gamelogic/SnakeTile.cpp)
source_group(gamelogic FILES ${SOURCE_GAMELOGIC_FILES})

set(SOURCE_RENDERING_FILES
	${SRC_DIR}/Rendering.hpp
	${SRC_DIR}/rendering/Assets.hpp
	${SRC_DIR}/rendering/Assets.cpp
	${SRC_DIR}/rendering/Camera.hpp
	${SRC_DIR}/rendering/Camera.cpp
	${SRC_DIR}/rendering/ClassicRenderer.hpp
	${SRC_DIR}/rendering/ClassicRenderer.cpp
	${SRC_DIR}/rendering/Materials.hpp
	${SRC_DIR}/rendering/Materials.cpp
	${SRC_DIR}/rendering/ModernRenderer.hpp
	${SRC_DIR}/rendering/ModernRenderer.cpp
	${SRC_DIR}/rendering/TileObject.hpp
	${SRC_DIR}/rendering/TileObject.cpp)
source_group(rendering FILES ${SOURCE_RENDERING_FILES})

set(SOURCE_SCREENS_FILES
	${SRC_DIR}/Screens.hpp
	${SRC_DIR}/screens/GameScreen.hpp
	${SRC_DIR}/screens/GameScreen.cpp
	${SRC_DIR}/screens/MainMenuScreen.hpp
	${SRC_DIR}/screens/MainMenuScreen.cpp
	${SRC_DIR}/screens/OptionsScreen.hpp
	${SRC_DIR}/screens/OptionsScreen.cpp
	${SRC_DIR}/screens/S3ItemRenderers.hpp
	${SRC_DIR}/screens/S3ItemRenderers.cpp
	${SRC_DIR}/screens/ScreenMenuConstants.hpp
	${SRC_DIR}/screens/ScreenUtils.hpp
	${SRC_DIR}/screens/ScreenUtils.cpp)
source_group(screens FILES ${SOURCE_SCREENS_FILES})

set(SOURCE_SFZ_GL_TEMP_FILES
	${SRC_DIR}/sfz/gl/BoxBlur.hpp
	${SRC_DIR}/sfz/gl/BoxBlur.cpp
	${SRC_DIR}/sfz/gl/Scaler.hpp
	${SRC_DIR}/sfz/gl/Scaler.cpp
	${SRC_DIR}/sfz/gl/Spotlight.hpp
	${SRC_DIR}/sfz/gl/Spotlight.cpp)
source_group(sfz_gl_temp FILES ${SOURCE_SFZ_GL_TEMP_FILES})

set(SOURCE_SFZ_GUI_TEMP_FILES
	${SRC_DIR}/sfz/GUI.hpp
	${SRC_DIR}/sfz/gui/BaseItem.hpp
	${SRC_DIR}/sfz/gui/Button.hpp
	${SRC_DIR}/sfz/gui/Button.cpp
	${SRC_DIR}/sfz/gui/DefaultItemRenderers.hpp
	${SRC_DIR}/sfz/gui/DefaultItemRenderers.cpp
	${SRC_DIR}/sfz/gui/GUIUtils.hpp
	${SRC_DIR}/sfz/gui/GUIUtils.cpp
	${SRC_DIR}/sfz/gui/ImageItem.hpp
	${SRC_DIR}/sfz/gui/ImageItem.cpp
	${SRC_DIR}/sfz/gui/InputData.hpp
	${SRC_DIR}/sfz/gui/ItemRenderer.hpp
	${SRC_DIR}/sfz/gui/MultiChoiceSelector.hpp
	${SRC_DIR}/sfz/gui/MultiChoiceSelector.cpp
	${SRC_DIR}/sfz/gui/OnOffSelector.hpp
	${SRC_DIR}/sfz/gui/OnOffSelector.cpp
	${SRC_DIR}/sfz/gui/ScrollListContainer.hpp
	${SRC_DIR}/sfz/gui/ScrollListContainer.cpp
	${SRC_DIR}/sfz/gui/SideSplitContainer.hpp
	${SRC_DIR}/sfz/gui/SideSplitContainer.cpp
	${SRC_DIR}/sfz/gui/System.hpp
	${SRC_DIR}/sfz/gui/System.cpp
	${SRC_DIR}/sfz/gui/TextItem.hpp
	${SRC_DIR}/sfz/gui/TextItem.cpp)
source_group(sfz_gui_temp FILES ${SOURCE_SFZ_GUI_TEMP_FILES})

set(SOURCE_SFZ_UTIL_TEMP_FILES
	${SRC_DIR}/sfz/util/FrametimeStats.hpp
	${SRC_DIR}/sfz/util/FrametimeStats.cpp)
source_group(sfz_util_temp FILES ${SOURCE_SFZ_UTIL_TEMP_FILES})

set(SOURCE_ALL_FILES
	${SOURCE_BASE_FILES}
	${SOURCE_GAMELOGIC_FILES}
	${SOURCE_RENDERING_FILES}
	${SOURCE_SCREENS_FILES}
	${SOURCE_SFZ_GL_TEMP_FILES}
	${SOURCE_SFZ_GUI_TEMP_FILES}
	${SOURCE_SFZ_UTIL_TEMP_FILES})

# Main executable
add_executable(snakium-cubed ${SOURCE_ALL_FILES})

# Linking libraries to main executable
target_link_libraries(
	snakium-cubed

	${SFZ_COMMON_LIBRARIES}
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
	# Create assets symlinks batch file
	file(COPY RuntimeDLLs/CREATE_ASSETS_SYMLINKS.BAT DESTINATION ${CMAKE_BINARY_DIR})

	# Runtime DLLs
	file(COPY RuntimeDLLs/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug)
	file(COPY RuntimeDLLs/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR}/Release)

else()
	# Create assets symlinks bash script (for non windows platforms)
	file(COPY RuntimeDLLs/CREATE_ASSETS_SYMLINKS.SH DESTINATION ${CMAKE_BINARY_DIR})
endif()