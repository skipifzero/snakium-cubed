#pragma once
#ifndef SNAKIUM_CUBED_SHADERS_HPP
#define SNAKIUM_CUBED_SHADERS_HPP

#include "sfz/GL.hpp"

namespace s3 {

/**
 * Shaders are compiled and returned by the functions in this class. The motivation for this is
 * that we currently don't have a good way of storing the shader source outside a c++ source file
 * without causing pain when the shaders are changed. This way we at least only need to recompile
 * one object file and not the whole Main.cpp when shaders are changed.
 */

GLuint compileStandardShaderProgram() noexcept;

} // namespace s3
#endif