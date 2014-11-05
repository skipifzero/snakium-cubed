cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
project( "snakium-cubed" )

# Directories
set( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )

add_executable( snakium-cubed
	${SOURCE_DIR}/Main.cpp
)