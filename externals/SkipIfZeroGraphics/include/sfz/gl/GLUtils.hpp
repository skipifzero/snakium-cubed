#pragma once
#ifndef SFZ_GL_GL_UTILS_HPP
#define SFZ_GL_GL_UTILS_HPP

#include <cstdint>
#include <string>

#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>

namespace gl {

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;
using sfz::mat3;
using sfz::mat4;

using std::string;
using std::uint32_t;

/**
 * @brief Checks the latest error returned by glGetError().
 * @return whether an error was found or not
 */
bool checkGLError() noexcept;

/**
 * @brief Checks all errors returned by glGetError() until GL_NO_ERROR is returned.
 * @return whether any errors where found or not
 */
bool checkAllGLErrors() noexcept;

/**
 * @brief Prints the error log returned by glShaderInfoLog().
 */
void printShaderInfoLog(uint32_t program) noexcept;

/**
 * @brief Creates and compiles a vertex shader with the specified source.
 * Terminates program if shader compilation failed.
 * @param shaderSource the source code of the vertex shader
 * @return the newly created and compiled vertex shader object
 */
uint32_t compileVertexShader(const string& shaderSource) noexcept; 

/**
 * @brief Creates and compiles a fragment shader with the specified source.
 * Terminates program if shader compilation failed.
 * @param shaderSource the source code of the fragment shader
 * @return the newly created and compiled fragment shader object
 */
uint32_t compileFragmentShader(const string& shaderSource) noexcept;

/**
 * @brief Links the specified program.
 * Terminates program if linking failed.
 * @param program the program
 */
void linkProgram(uint32_t program) noexcept;

// Uniform setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setUniform(int location, int i) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, int i) noexcept;
void setUniform(int location, const int* intArray, size_t count) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const int* intArray, size_t count) noexcept;

void setUniform(int location, float f) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, float f) noexcept;
void setUniform(int location, const float* floatArray, size_t count) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const float* floatArray, size_t count) noexcept;

void setUniform(int location, vec2 vector) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, vec2 vector) noexcept;
void setUniform(int location, const vec2* vectorArray, size_t count) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const vec2* vectorArray, size_t count) noexcept;

void setUniform(int location, const vec3& vector) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const vec3& vector) noexcept;
void setUniform(int location, const vec3* vectorArray, size_t count) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const vec3* vectorArray, size_t count) noexcept;

void setUniform(int location, const vec4& vector) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const vec4& vector) noexcept;
void setUniform(int location, const vec4* vectorArray, size_t count) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const vec4* vectorArray, size_t count) noexcept;

void setUniform(int location, const mat3& matrix) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const mat3& matrix) noexcept;
void setUniform(int location, const mat3* matrixArray, size_t count) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const mat3* matrixArray, size_t count) noexcept;

void setUniform(int location, const mat4& matrix) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const mat4& matrix) noexcept;
void setUniform(int location, const mat4* matrixArray, size_t count) noexcept;
void setUniform(uint32_t shaderProgram, const string& name, const mat4* matrixArray, size_t count) noexcept;

} // namespace gl
#endif
