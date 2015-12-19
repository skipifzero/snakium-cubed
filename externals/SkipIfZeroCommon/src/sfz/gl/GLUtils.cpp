#include "sfz/gl/GLUtils.hpp"

#include <iostream>

#include "sfz/Assert.hpp"
#include "sfz/gl/OpenGL.hpp"

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* debugSourceEnumToString(GLenum source) noexcept
{
	switch (source) {
	case GL_DEBUG_SOURCE_API: return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM";
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
	case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY";
	case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
	case GL_DEBUG_SOURCE_OTHER: return "OTHER";
	}
	return "UNKNOWN";
}

static const char* debugTypeEnumToString(GLenum type) noexcept
{
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: return "ERROR";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
	case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
	case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
	case GL_DEBUG_TYPE_MARKER: return "MARKER";
	case GL_DEBUG_TYPE_PUSH_GROUP: return "PUSH_GROUP";
	case GL_DEBUG_TYPE_POP_GROUP: return "POP_GROUP";
	case GL_DEBUG_TYPE_OTHER: return "OTHER";
	}
	return "UNKNOWN";
}

static const char* debugSeverityEnumToString(GLenum severity) noexcept
{
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
	case GL_DEBUG_SEVERITY_LOW: return "LOW";
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
	}
	return "UNKNOWN";
}

#if !defined(APIENTRY)
#define APIENTRY
#endif

static Severity breakLevel;

static void APIENTRY printDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
	// Filter out some useless messages
	switch (id) {
	case 131218: return; // Program/shader state performance warning: Fragment shader in program X is being recompiled based on GL state.
	}

	// Print message to stderr
	std::cerr << "OpenGL Debug Message:"
	          << "\nSource: " << debugSourceEnumToString(source)
	          << "\nType: " << debugTypeEnumToString(type)
	          << "\nID: " << id
	          << "\nSeverity: " << debugSeverityEnumToString(severity)
	          << "\nMessage: " << message
	          << std::endl << std::endl;

	// Breaks if message is of high enough severity
	switch (breakLevel) {
	case Severity::NOTIFICATION:
		sfz_assert_debug(false);
		break;
	case Severity::LOW:
		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
			sfz_assert_debug(false);
		}
		break;
	case Severity::MEDIUM:
		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION && severity != GL_DEBUG_SEVERITY_LOW) {
			sfz_assert_debug(false);
		}
		break;
	case Severity::HIGH:
		if (severity == GL_DEBUG_SEVERITY_HIGH) {
			sfz_assert_debug(false);
		}
		break;
	case Severity::NONE:
		// Break on nothing
		break;
	}
}

// Functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void printSystemGLInfo() noexcept
{
	std::cout << "Vendor: " << glGetString(GL_VENDOR)
	          << "\nVersion: " << glGetString(GL_VERSION)
	          << "\nRenderer: " << glGetString(GL_RENDERER)
	          << std::endl << std::endl;
}

// OpenGL debug message functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void setupDebugMessages(Severity acceptLevel, Severity breakLevel) noexcept
{
	if (!glewIsExtensionSupported("GL_ARB_debug_output")) {
		std::cerr << "GL_ARB_debug_output is not supported on this configuration!\n";
		return;
	}

	glDebugMessageCallback(printDebugMessage, nullptr);

	setDebugMessageAcceptLevel(acceptLevel);
	setDebugMessageBreakLevel(breakLevel);

	// Ensures messages are called synchronously from same thread
	// Pros: Stack-trace will be useful
	// Cons: Performance loss
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void setDebugMessageAcceptLevel(Severity level) noexcept
{
	switch (level) {
	case Severity::NOTIFICATION:
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
		break;
	case Severity::LOW:
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
		break;
	case Severity::MEDIUM:
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, 0, GL_FALSE);
		break;
	case Severity::HIGH:
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, 0, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, 0, GL_FALSE);
		break;
	case Severity::NONE:
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_FALSE);
		break;
	}
}

void setDebugMessageBreakLevel(Severity level) noexcept
{
	breakLevel = level;
}

} // namespace gl