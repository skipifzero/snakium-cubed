#include "sfz/gl/Utils.hpp"

namespace gl {

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

void printShaderInfoLog(GLuint program) noexcept
{
	int logLength;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &logLength);
	char* log = new char[logLength+1];
	glGetShaderInfoLog(program, logLength, NULL, log);
	std::cout << log << std::endl;
	delete[] log;
}

GLuint compileVertexShader(const std::string& shaderSource) noexcept
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* shaderSourcePtr = shaderSource.c_str();
	glShaderSource(vertexShader, 1, &shaderSourcePtr, NULL);
	glCompileShader(vertexShader);
	{
		int compileSuccess;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileSuccess);
		if (!compileSuccess) {
			printShaderInfoLog(vertexShader);
			checkAllGLErrors();
			std::terminate();
		}
	}
	checkAllGLErrors();
	return vertexShader;
}

GLuint compileFragmentShader(const std::string& shaderSource) noexcept
{
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* shaderSourcePtr = shaderSource.c_str();
	glShaderSource(fragmentShader, 1, &shaderSourcePtr, NULL);
	glCompileShader(fragmentShader);
	{
		int compileSuccess;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileSuccess);
		if (!compileSuccess) {
			printShaderInfoLog(fragmentShader);
			checkAllGLErrors();
			std::terminate();
		}
	}
	checkAllGLErrors();
	return fragmentShader;
}

void linkProgram(GLuint program) noexcept
{
	glLinkProgram(program);
	{
		GLint linkSuccess = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
		if (!linkSuccess) {
			printShaderInfoLog(program);
			checkAllGLErrors();
			std::terminate();
		}
	}
	checkAllGLErrors();
}

} // namespace gl