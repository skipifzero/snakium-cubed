#include "sfz/gl/GLUtils.hpp"

#include <iostream>

#include "sfz/gl/OpenGL.hpp"

namespace gl {

bool checkGLError() noexcept
{
	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		std::cerr << "OpenGL error " << errorCode << ", " << gluErrorString(errorCode)
		          << std::endl;
		return true;
	}
	return false;
}

bool checkAllGLErrors() noexcept
{
	bool foundError = false;
	GLenum errorCode = glGetError();
	while (errorCode != GL_NO_ERROR) {
		std::cerr << "OpenGL error " << errorCode << ", " << gluErrorString(errorCode)
		          << std::endl;
		errorCode = glGetError();
		foundError = true;
	}
	return foundError;
}

} // namespace gl