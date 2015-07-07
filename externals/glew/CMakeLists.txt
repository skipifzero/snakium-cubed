add_definitions(
	-DGLEW_STATIC
)

find_package(OpenGL REQUIRED) # ${OPENGL_INCLUDE_DIR}, ${OPENGL_LIBRARIES}

# Compile glew
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(GLEW_1_11_0 
	STATIC 
	src/glew.cpp)

target_link_libraries(GLEW_1_11_0
	${OPENGL_LIBRARIES}
)

# Variables for linking
set(GLEW_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
set(GLEW_LIBRARIES GLEW_1_11_0 PARENT_SCOPE)