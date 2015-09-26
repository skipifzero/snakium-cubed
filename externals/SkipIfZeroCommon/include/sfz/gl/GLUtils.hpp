#pragma once
#ifndef SFZ_GL_GL_UTILS_HPP
#define SFZ_GL_GL_UTILS_HPP

namespace gl {

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

} // namespace gl
#endif
