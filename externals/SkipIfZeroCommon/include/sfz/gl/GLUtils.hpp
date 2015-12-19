#pragma once
#ifndef SFZ_GL_GL_UTILS_HPP
#define SFZ_GL_GL_UTILS_HPP

namespace gl {

/**
 * @brief Prints system information to stdout.
 */
void printSystemGLInfo() noexcept;

// OpenGL debug message functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Severity level for OpenGL debug messages
 * Ordered by "severity", NOTIFICATION < LOW < MEDIUM < HIGH < NONE.
 * Specifying NOTIFICATION means all messages will be accepted, NONE means no messages will be.
 */
enum class Severity {
	NOTIFICATION,
	LOW,
	MEDIUM,
	HIGH,
	NONE
};

/**
 * @brief Setups so OpenGL debug messages will be printed to stderr
 * Requires an OpenGL debug context to function properly. Should be called after OpenGL 
 * extension loading is done.
 * @param acceptLevel the lowest level of message severity accepted and printed
 * @param breakLevel the lowest level of message severity which will break to debugger
 */
void setupDebugMessages(Severity acceptLevel = Severity::MEDIUM,
                        Severity breakLevel = Severity::HIGH) noexcept;

/**
 * @brief Sets the lowest level of message severity accepted and printed
 */
void setDebugMessageAcceptLevel(Severity level) noexcept;

/**
* @brief Sets the lowest level of message severity which will break to debugger
*/
void setDebugMessageBreakLevel(Severity level) noexcept;

} // namespace gl
#endif
