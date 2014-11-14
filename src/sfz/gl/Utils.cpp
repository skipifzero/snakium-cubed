#include "sfz/gl/Utils.hpp"

namespace sfz {

void checkGLError() noexcept
{
	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		std::cout << "OpenGL Error: " << gluErrorString(errorCode) << std::endl;
	}
}

void checkAllGLErrors() noexcept
{
	GLenum errorCode = glGetError();
	while (errorCode != GL_NO_ERROR) {
		std::cout << "OpenGL Error: " << gluErrorString(errorCode) << std::endl;
		errorCode = glGetError();
	}
}

} // namespace sfz