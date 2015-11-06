#include "sfz/gl/Program.hpp"

#include <algorithm>
#include <iostream>
#include <new>

#include "sfz/gl/OpenGL.hpp"
#include "sfz/util/IO.hpp"

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* POST_PROCESS_VERTEX_SHADER_SOURCE = R"(
	#version 330

	// Input
	in vec3 inPosition;
	in vec3 inNormal;
	in vec2 inUV;
	in int inMaterialID;

	// Output
	out vec2 uvCoord;

	void main()
	{
		gl_Position = vec4(inPosition, 1.0);
		uvCoord = inUV;
	}
)";

// Program: Constructor functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Program Program::fromSource(const char* vertexSrc, const char* geometrySrc, const char* fragmentSrc,
                            void(*bindAttribFragFunc)(uint32_t shaderProgram)) noexcept
{
	GLuint vertexShader = compileShader(vertexSrc, GL_VERTEX_SHADER);
	if (vertexShader == 0) {
		std::cerr << "Couldn't compile vertex shader." << std::endl;
		return Program{};
	}

	GLuint geometryShader = compileShader(geometrySrc, GL_GEOMETRY_SHADER);
	if (geometryShader == 0) {
		std::cerr << "Couldn't compile geometry shader." << std::endl;
		return Program{};
	}
	
	GLuint fragmentShader = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) {
		std::cerr << "Couldn't compile fragment shader." << std::endl;
		return Program{};
	}

	GLuint shaderProgram = glCreateProgram();
	
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, geometryShader);
	glAttachShader(shaderProgram, fragmentShader);

	// glBindAttribLocation() & glBindFragDataLocation()
	if (bindAttribFragFunc != nullptr) bindAttribFragFunc(shaderProgram);

	bool linkSuccess = linkProgram(shaderProgram);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, geometryShader);
	glDetachShader(shaderProgram, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	if (!linkSuccess) {
		glDeleteProgram(shaderProgram);
		std::cerr << "Couldn't link shader program." << std::endl;
		return Program{};
	}
	
	Program temp;
	temp.mHandle = shaderProgram;
	temp.mBindAttribFragFunc = bindAttribFragFunc;
	return std::move(temp);
}

Program Program::fromSource(const char* vertexSrc, const char* fragmentSrc,
                            void(*bindAttribFragFunc)(uint32_t shaderProgram)) noexcept
{
	GLuint vertexShader = compileShader(vertexSrc, GL_VERTEX_SHADER);
	if (vertexShader == 0) {
		std::cerr << "Couldn't compile vertex shader." << std::endl;
		return Program{};
	}
	
	GLuint fragmentShader = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) {
		std::cerr << "Couldn't compile fragment shader." << std::endl;
		return Program{};
	}

	GLuint shaderProgram = glCreateProgram();
	
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// glBindAttribLocation() & glBindFragDataLocation()
	if (bindAttribFragFunc != nullptr) bindAttribFragFunc(shaderProgram);

	bool linkSuccess = linkProgram(shaderProgram);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (!linkSuccess) {
		glDeleteProgram(shaderProgram);
		std::cerr << "Couldn't link shader program." << std::endl;
		return Program{};
	}
	
	Program temp;
	temp.mHandle = shaderProgram;
	temp.mBindAttribFragFunc = bindAttribFragFunc;
	return temp;
}

Program Program::postProcessFromSource(const char* postProcessSource) noexcept
{
	Program tmp;
	tmp = Program::fromSource(POST_PROCESS_VERTEX_SHADER_SOURCE, postProcessSource, [](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "inPosition");
		glBindAttribLocation(shaderProgram, 1, "inNormal");
		glBindAttribLocation(shaderProgram, 2, "inUV");
		glBindAttribLocation(shaderProgram, 3, "inMaterialID");
	});
	tmp.mIsPostProcess = true;
	return std::move(tmp);
}


Program Program::fromFile(const char* vertexPath, const char* geometryPath, const char* fragmentPath,
                          void(*bindAttribFragFunc)(uint32_t shaderProgram)) noexcept
{
	Program tmp;
	tmp.mVertexPath = vertexPath;
	tmp.mGeometryPath = geometryPath;
	tmp.mFragmentPath = fragmentPath;
	tmp.mBindAttribFragFunc = bindAttribFragFunc;
	tmp.reload();
	tmp.mWasReloaded = false;
	return tmp;
}

Program Program::fromFile(const char* vertexPath, const char* fragmentPath,
                          void(*bindAttribFragFunc)(uint32_t shaderProgram)) noexcept
{
	Program tmp;
	tmp.mVertexPath = vertexPath;
	tmp.mFragmentPath = fragmentPath;
	tmp.mBindAttribFragFunc = bindAttribFragFunc;
	tmp.reload();
	tmp.mWasReloaded = false;
	return tmp;
}

Program Program::postProcessFromFile(const char* postProcessPath) noexcept
{
	Program tmp;
	tmp.mFragmentPath = postProcessPath;
	tmp.mIsPostProcess = true;
	tmp.reload();
	tmp.mWasReloaded = false;
	return tmp;
}

// Program: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool Program::reload() noexcept
{
	const string vertexSrc = sfz::readTextFile(mVertexPath.c_str());
	const string geometrySrc = sfz::readTextFile(mGeometryPath.c_str());
	const string fragmentSrc = sfz::readTextFile(mFragmentPath.c_str());

	if (mIsPostProcess && (fragmentSrc.size() > 0)) {
		Program tmp = Program::postProcessFromSource(fragmentSrc.c_str());
		if (!tmp.isValid()) return false;

		tmp.mFragmentPath = this->mFragmentPath;
		tmp.mIsPostProcess = true;
		tmp.mWasReloaded = true;
		*this = std::move(tmp);
		return true;
	}
	else if ((vertexSrc.size() > 0) && (geometrySrc.size() > 0) && (fragmentSrc.size() > 0)) {
		Program tmp = Program::fromSource(vertexSrc.c_str(), geometrySrc.c_str(), fragmentSrc.c_str(),
		                                  mBindAttribFragFunc);
		if (!tmp.isValid()) return false;

		tmp.mVertexPath = this->mVertexPath;
		tmp.mGeometryPath = this->mGeometryPath;
		tmp.mFragmentPath = this->mFragmentPath;
		tmp.mBindAttribFragFunc = this->mBindAttribFragFunc;
		tmp.mWasReloaded = true;
		*this = std::move(tmp);
		return true;
	}
	else if ((vertexSrc.size() > 0) && (fragmentSrc.size() > 0)) {
		Program tmp = Program::fromSource(vertexSrc.c_str(), fragmentSrc.c_str(), mBindAttribFragFunc);
		if (!tmp.isValid()) return false;

		tmp.mVertexPath = this->mVertexPath;
		tmp.mFragmentPath = this->mFragmentPath;
		tmp.mBindAttribFragFunc = this->mBindAttribFragFunc;
		tmp.mWasReloaded = true;
		*this = std::move(tmp);
		return true;
	}

	return false;
}


// Program: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Program::Program(Program&& other) noexcept
{
	std::swap(this->mHandle, other.mHandle);
	std::swap(this->mVertexPath, other.mVertexPath);
	std::swap(this->mGeometryPath, other.mGeometryPath);
	std::swap(this->mFragmentPath, other.mFragmentPath);
	std::swap(this->mIsPostProcess, other.mIsPostProcess);
	std::swap(this->mWasReloaded, other.mWasReloaded);
	std::swap(this->mBindAttribFragFunc, other.mBindAttribFragFunc);
}

Program& Program::operator= (Program&& other) noexcept
{
	std::swap(this->mHandle, other.mHandle);
	std::swap(this->mVertexPath, other.mVertexPath);
	std::swap(this->mGeometryPath, other.mGeometryPath);
	std::swap(this->mFragmentPath, other.mFragmentPath);
	std::swap(this->mIsPostProcess, other.mIsPostProcess);
	std::swap(this->mWasReloaded, other.mWasReloaded);
	std::swap(this->mBindAttribFragFunc, other.mBindAttribFragFunc);
	return *this;
}

Program::~Program() noexcept
{
	glDeleteProgram(mHandle); // Silently ignored if mHandle == 0.
}

// Program compilation & linking helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

uint32_t compileShader(const char* source, uint32_t shaderType) noexcept
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int compileSuccess;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
	if (!compileSuccess) {
		printShaderInfoLog(shader);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool linkProgram(uint32_t program) noexcept
{
	glLinkProgram(program);
	GLint linkSuccess = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess) {
		printShaderInfoLog(program);
		return false;
	}
	return true;
}

void printShaderInfoLog(uint32_t shader) noexcept
{
	int logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	char* log = new (std::nothrow) char[(size_t)logLength+1];
	glGetShaderInfoLog(shader, logLength, NULL, log);
	std::cerr << log << std::endl;
	delete[] log;
}

const char* postProcessVertexShaderSource() noexcept
{
	return POST_PROCESS_VERTEX_SHADER_SOURCE;
}

// Uniform setters: int
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, int i) noexcept
{
	glUniform1i(location, i);
}

void setUniform(const Program& program, const char* name, int i) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, i);
}

void setUniform(int location, const int* intArray, size_t count) noexcept
{
	glUniform1iv(location, count, intArray);
}

void setUniform(const Program& program, const char* name, const int* intArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, intArray, count);
}

// Uniform setters: float
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, float f) noexcept
{
	glUniform1f(location, f);
}

void setUniform(const Program& program, const char* name, float f) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, f);
}

void setUniform(int location, const float* floatArray, size_t count) noexcept
{
	glUniform1fv(location, count, floatArray);
}

void setUniform(const Program& program, const char* name, const float* floatArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, floatArray, count);
}

// Uniform setters: vec2
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, vec2 vector) noexcept
{
	glUniform2fv(location, 1, vector.elements);
}

void setUniform(const Program& program, const char* name, vec2 vector) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, vector);
}

void setUniform(int location, const vec2* vectorArray, size_t count) noexcept
{
	static_assert(sizeof(vec2) == sizeof(float)*2, "vec2 is padded");
	glUniform2fv(location, count, vectorArray[0].elements);
}

void setUniform(const Program& program, const char* name, const vec2* vectorArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, vectorArray, count);
}

// Uniform setters: vec3
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const vec3& vector) noexcept
{
	glUniform3fv(location, 1, vector.elements);
}

void setUniform(const Program& program, const char* name, const vec3& vector) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, vector);
}

void setUniform(int location, const vec3* vectorArray, size_t count) noexcept
{
	static_assert(sizeof(vec3) == sizeof(float)*3, "vec3 is padded");
	glUniform3fv(location, count, vectorArray[0].elements);
}

void setUniform(const Program& program, const char* name, const vec3* vectorArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, vectorArray, count);
}

// Uniform setters: vec4
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const vec4& vector) noexcept
{
	glUniform4fv(location, 1, vector.elements);
}

void setUniform(const Program& program, const char* name, const vec4& vector) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, vector);
}

void setUniform(int location, const vec4* vectorArray, size_t count) noexcept
{
	static_assert(sizeof(vec4) == sizeof(float)*4, "vec4 is padded");
	glUniform4fv(location, count, vectorArray[0].elements);
}

void setUniform(const Program& program, const char* name, const vec4* vectorArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, vectorArray, count);
}

// Uniform setters: mat3
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const mat3& matrix) noexcept
{
	glUniformMatrix3fv(location, 1, false, matrix.data());
}

void setUniform(const Program& program, const char* name, const mat3& matrix) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, matrix);
}

void setUniform(int location, const mat3* matrixArray, size_t count) noexcept
{
	static_assert(sizeof(mat3) == sizeof(float)*9, "mat3 is padded");
	glUniformMatrix3fv(location, count, false, matrixArray[0].data());
}

void setUniform(const Program& program, const char* name, const mat3* matrixArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, matrixArray, count);
}

// Uniform setters: mat4
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, const mat4& matrix) noexcept
{
	glUniformMatrix4fv(location, 1, false, matrix.data());
}

void setUniform(const Program& program, const char* name, const mat4& matrix) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, matrix);
}

void setUniform(int location, const mat4* matrixArray, size_t count) noexcept
{
	static_assert(sizeof(mat4) == sizeof(float)*16, "mat4 is padded");
	glUniformMatrix4fv(location, count, false, matrixArray[0].data());
}

void setUniform(const Program& program, const char* name, const mat4* matrixArray, size_t count) noexcept
{
	int loc = glGetUniformLocation(program.handle(), name);
	setUniform(loc, matrixArray, count);
}

} // namespace gl