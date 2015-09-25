#pragma once
#ifndef SFZ_GL_SHADER_PROGRAM_HPP
#define SFZ_GL_SHADER_PROGRAM_HPP

#include <cstdint>
#include <string>

namespace gl {

using std::string;
using std::uint32_t;

// Program class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Program final {
public:
	// Constructor functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Program fromSource(const char* vertexSrc, const char* geometrySrc, const char* fragmentSrc) noexcept;
	Program fromSource(const string& vertexSrc, const string& geometrySrc, const string& fragmentSrc) noexcept;
	Program fromSource(const char* vertexSrc, const char* fragmentSrc) noexcept;
	Program fromSource(const string& vertexSrc, const string& fragmentSrc) noexcept;
	Program postProcessFromSource(const char* postProcessSrc) noexcept;
	Program postProcessFromSource(const string& postProcessSrc) noexcept;

	Program fromFile(const char* vertexPath, const char* geometryPath, const char* fragmentPath) noexcept;
	Program fromFile(const string& vertexPath, const string& geometryPath, const string& fragmentPath) noexcept;
	Program fromFile(const char* vertexPath, const char* fragmentPath) noexcept;
	Program fromFile(const string& vertexPath, const string& fragmentPath) noexcept;
	Program postProcessFromFile(const char* postProcessPath) noexcept;
	Program postProcessFromFile(const string& postProcessPath) noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline uint32_t handle() const noexcept { return mHandle; }
	inline bool isValid() const noexcept { return (mHandle != 0); }
	bool reload() noexcept;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Program(const Program&) = delete;
	Program& operator= (const Program&) = delete;

	Program() noexcept;
	Program(Program&& other) noexcept;
	Program& operator= (Program&& other) noexcept;
	~Program() noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mHandle = 0;
	string mVertexPath, mGeometryPath, mFragmentPath;
};

} // namespace gl
#endif