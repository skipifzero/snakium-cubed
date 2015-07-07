#include "sfz/gl/Utils.hpp"



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

void printShaderInfoLog(GLuint program) noexcept
{
	int logLength;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &logLength);
	char* log = new char[logLength+1];
	glGetShaderInfoLog(program, logLength, NULL, log);
	std::cerr << log << std::endl;
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

// Uniform setters: int
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, int i) noexcept
{
	glUniform1i(location, i);
}

void setUniform(GLuint shaderProgram, const string& name, int i) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, i);
}

void setUniform(int location, const int* intArray, size_t count) noexcept
{
	glUniform1iv(location, count, intArray);
}

void setUniform(GLuint shaderProgram, const string& name, const int* intArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, intArray, count);
}

// Uniform setters: float
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, float f) noexcept
{
	glUniform1f(location, f);
}

void setUniform(GLuint shaderProgram, const string& name, float f) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, f);
}

void setUniform(int location, const float* floatArray, size_t count) noexcept
{
	glUniform1fv(location, count, floatArray);
}

void setUniform(GLuint shaderProgram, const string& name, const float* floatArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, floatArray, count);
}

// Uniform setters: vec2
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, vec2 vector) noexcept
{
	glUniform2fv(location, 1, vector.elements);
}

void setUniform(GLuint shaderProgram, const string& name, vec2 vector) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, vector);
}

void setUniform(int location, const vec2* vectorArray, size_t count) noexcept
{
	static_assert(sizeof(vec2) == sizeof(float)*2, "vec2 is padded");
	glUniform2fv(location, count, vectorArray[0].elements);
}

void setUniform(GLuint shaderProgram, const string& name, const vec2* vectorArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, vectorArray, count);
}

// Uniform setters: vec3
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const vec3& vector) noexcept
{
	glUniform3fv(location, 1, vector.elements);
}

void setUniform(GLuint shaderProgram, const string& name, const vec3& vector) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, vector);
}

void setUniform(int location, const vec3* vectorArray, size_t count) noexcept
{
	static_assert(sizeof(vec3) == sizeof(float)*3, "vec3 is padded");
	glUniform3fv(location, count, vectorArray[0].elements);
}

void setUniform(GLuint shaderProgram, const string& name, const vec3* vectorArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, vectorArray, count);
}

// Uniform setters: vec4
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const vec4& vector) noexcept
{
	glUniform4fv(location, 1, vector.elements);
}

void setUniform(GLuint shaderProgram, const string& name, const vec4& vector) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, vector);
}

void setUniform(int location, const vec4* vectorArray, size_t count) noexcept
{
	static_assert(sizeof(vec4) == sizeof(float)*4, "vec4 is padded");
	glUniform4fv(location, count, vectorArray[0].elements);
}

void setUniform(GLuint shaderProgram, const string& name, const vec4* vectorArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, vectorArray, count);
}

// Uniform setters: mat3
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const mat3& matrix) noexcept
{
	glUniformMatrix3fv(location, 1, false, matrix.data());
}

void setUniform(GLuint shaderProgram, const string& name, const mat3& matrix) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, matrix);
}

void setUniform(int location, const mat3* matrixArray, size_t count) noexcept
{
	static_assert(sizeof(mat3) == sizeof(float)*9, "mat3 is padded");
	glUniformMatrix3fv(location, count, false, matrixArray[0].data());
}

void setUniform(GLuint shaderProgram, const string& name, const mat3* matrixArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, matrixArray, count);
}

// Uniform setters: mat4
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const mat4& matrix) noexcept
{
	glUniformMatrix4fv(location, 1, false, matrix.data());
}

void setUniform(GLuint shaderProgram, const string& name, const mat4& matrix) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, matrix);
}

void setUniform(int location, const mat4* matrixArray, size_t count) noexcept
{
	static_assert(sizeof(mat4) == sizeof(float)*16, "mat4 is padded");
	glUniformMatrix4fv(location, count, false, matrixArray[0].data());
}

void setUniform(GLuint shaderProgram, const string& name, const mat4* matrixArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, matrixArray, count);
}

} // namespace gl

