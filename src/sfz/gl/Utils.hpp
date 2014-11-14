#pragma once
#ifndef SFZ_GL_UTILS_HPP
#define SFZ_GL_UTILS_HPP

#include <gl/glew.h>
#include <iostream>

namespace sfz {

/**
 * @brief Checks the latest error returned by glGetError().
 */
void checkGLError() noexcept;

/**
 * @brief Checks all errors returned by glGetError() until GL_NO_ERROR is returned.
 */
void checkAllGLErrors() noexcept;

} // namespace sfz

#endif
