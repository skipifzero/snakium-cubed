#include "sfz/gl/Program.hpp"

#include <algorithm>
#include <iostream>
#include <new>

#include "sfz/gl/OpenGL.hpp"
#include "sfz/util/IO.hpp"

namespace gl {

// Static function
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static void printShaderInfoLog(GLuint shader) noexcept
{
	int logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	char* log = new (std::nothrow) char[(size_t)logLength+1];
	glGetShaderInfoLog(shader, logLength, NULL, log);
	std::cerr << log << std::endl;
	delete[] log;
}

/* Returns 0 if shader compilation failed. */
static GLuint compileShader(const char* source, GLenum shaderType) noexcept
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

/* Returns false if linking failed. */
static bool linkProgram(GLuint program) noexcept
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

// Program: Constructor functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Program Program::fromSource(const char* vertexSrc, const char* geometrySrc, const char* fragmentSrc) noexcept
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

	// TODO: glBindAttribLocation() & glBindFragDataLocation() here?

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
	return std::move(temp);
}

Program Program::fromSource(const string& vertexSrc, const string& geometrySrc, const string& fragmentSrc) noexcept
{
	return fromSource(vertexSrc.c_str(), geometrySrc.c_str(), fragmentSrc.c_str());
}

Program Program::fromSource(const char* vertexSrc, const char* fragmentSrc) noexcept
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

	// TODO: glBindAttribLocation() & glBindFragDataLocation() here?

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
	return temp;
}

Program Program::fromSource(const string& vertexSrc, const string& fragmentSrc) noexcept
{
	return fromSource(vertexSrc.c_str(), fragmentSrc.c_str());
}

Program Program::postProcessFromSource(const char* postProcessSrc) noexcept
{
	// TODO: Implement
	return Program{};
}

Program Program::postProcessFromSource(const string& postProcessSrc) noexcept
{
	return postProcessFromSource(postProcessSrc.c_str());
}


Program Program::fromFile(const char* vertexPath, const char* geometryPath, const char* fragmentPath) noexcept
{
	Program tmp;
	tmp.mVertexPath = vertexPath;
	tmp.mGeometryPath = geometryPath;
	tmp.mFragmentPath = fragmentPath;
	tmp.reload();
	return tmp;
}

Program Program::fromFile(const string& vertexPath, const string& geometryPath, const string& fragmentPath) noexcept
{
	return fromFile(vertexPath.c_str(), geometryPath.c_str(), fragmentPath.c_str());
}

Program Program::fromFile(const char* vertexPath, const char* fragmentPath) noexcept
{
	Program tmp;
	tmp.mVertexPath = vertexPath;
	tmp.mFragmentPath = fragmentPath;
	tmp.reload();
	return tmp;
}

Program Program::fromFile(const string& vertexPath, const string& fragmentPath) noexcept
{
	return fromFile(vertexPath.c_str(), fragmentPath.c_str());
}

Program Program::postProcessFromFile(const char* postProcessPath) noexcept
{
	// TODO: Implement
	return Program{};
}

Program Program::postProcessFromFile(const string& postProcessPath) noexcept
{
	return postProcessFromFile(postProcessPath.c_str());
}

// Program: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool Program::reload() noexcept
{
	const string vertexSrc = sfz::readTextFile(mVertexPath.c_str());
	const string geometrySrc = sfz::readTextFile(mGeometryPath.c_str());
	const string fragmentSrc = sfz::readTextFile(mFragmentPath.c_str());

	if ((vertexSrc.size() > 0) && (geometrySrc.size() > 0) && (fragmentSrc.size() > 0)) {
		Program tmp = Program::fromSource(vertexSrc, geometrySrc, fragmentSrc);
		if (!tmp.isValid()) return false;
		*this = std::move(tmp);
		return true;
	}
	else if ((vertexSrc.size() > 0) && (fragmentSrc.size() > 0)) {
		Program tmp = Program::fromSource(vertexSrc, fragmentSrc);
		if (!tmp.isValid()) return false;
		*this = std::move(tmp);
		return true;
	}

	// TODO: Handle post-process shaders

	return false;
}


// Program: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Program::Program() noexcept
:
	mVertexPath{""},
	mGeometryPath{""},
	mFragmentPath{""}
{ }

Program::Program(Program&& other) noexcept
{
	std::swap(this->mHandle, other.mHandle);
	std::swap(this->mVertexPath, other.mVertexPath);
	std::swap(this->mGeometryPath, other.mGeometryPath);
	std::swap(this->mFragmentPath, other.mFragmentPath);
}

Program& Program::operator= (Program&& other) noexcept
{
	std::swap(this->mHandle, other.mHandle);
	std::swap(this->mVertexPath, other.mVertexPath);
	std::swap(this->mGeometryPath, other.mGeometryPath);
	std::swap(this->mFragmentPath, other.mFragmentPath);
	return *this;
}

Program::~Program() noexcept
{
	glDeleteProgram(mHandle); // Silently ignored if mHandle == 0.
}

} // namespace gl