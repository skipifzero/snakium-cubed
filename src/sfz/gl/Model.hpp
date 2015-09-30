#pragma once
#ifndef SFZ_GL_MODEL_HPP
#define SFZ_GL_MODEL_HPP

#include <cstddef>
#include <cstdint>
#include <memory>

namespace gl {

using std::size_t;
using std::uint32_t;
using std::unique_ptr;

/**
 * AttribLocation[0] = Position (vec3)
 * AttribLocation[1] = Normal (vec3)
 * AttribLocation[2] = UV Coords (vec2)
 * AttribLocation[3] = Material ID (int)
 */
class Model final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	Model() noexcept = default;
	Model(const Model&) = delete;
	Model& operator= (const Model&) = delete;
	~Model() noexcept = default;
	
	Model(const char* basePath, const char* filename, bool requireNormals = true,
	      bool requireUVs = true) noexcept;
	Model(Model&& other) noexcept;
	Model& operator= (Model&& other) noexcept;
	
	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void render() noexcept;

private:
	// Private classes
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	struct VAORenderingInfo final {
		uint32_t vao = 0;
		uint32_t indexBuffer = 0;
		size_t numIndices = 0;

		VAORenderingInfo() noexcept = default;
		VAORenderingInfo(const VAORenderingInfo&) = delete;
		VAORenderingInfo& operator= (const VAORenderingInfo&) = delete;

		VAORenderingInfo(VAORenderingInfo&& other) noexcept;
		VAORenderingInfo& operator= (VAORenderingInfo&& other) noexcept;
		~VAORenderingInfo() noexcept;
	};

	struct VAOBufferInfo final {
		uint32_t positionBuffer = 0;
		uint32_t normalBuffer = 0;
		uint32_t uvBuffer = 0;
		uint32_t materialIDBuffer = 0;

		VAOBufferInfo() noexcept = default;
		VAOBufferInfo(const VAOBufferInfo&) = delete;
		VAOBufferInfo& operator= (const VAOBufferInfo&) = delete;

		VAOBufferInfo(VAOBufferInfo&& other) noexcept;
		VAOBufferInfo& operator= (VAOBufferInfo&& other) noexcept;
		~VAOBufferInfo() noexcept;
	};

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	unique_ptr<VAORenderingInfo[]> mVAORenderingInfos = nullptr;
	unique_ptr<VAOBufferInfo[]> mVAOBufferInfos = nullptr; // Destroyed before mVAORenderingInfos
	size_t mNumVAOs = 0;
};

} // namespace gl
#endif