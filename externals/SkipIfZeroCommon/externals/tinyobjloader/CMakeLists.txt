
# Compile tinyobjloader
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(tinyobjloader
	${CMAKE_CURRENT_SOURCE_DIR}/include/tiny_obj_loader.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/tiny_obj_loader.cc)

# Variables for linking
set(TINYOBJLOADER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
set(TINYOBJLOADER_LIBRARIES tinyobjloader PARENT_SCOPE)