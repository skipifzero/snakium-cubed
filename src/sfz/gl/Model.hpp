#pragma once
#ifndef SFZ_GL_MODEL_HPP
#define SFZ_GL_MODEL_HPP

#include <cstddef>
#include <cstdint>

namespace gl {

using std::size_t;
using std::uint32_t;

class Model final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	Model() noexcept = default;
	Model(const Model&) = delete;
	Model& operator= (const Model&) = delete;
	
	Model(const char* basePath, const char* filename) noexcept;
	Model(Model&& other) noexcept;
	Model& operator= (Model&& other) noexcept;
	~Model() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void render() noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mVAO = 0;
	uint32_t mPositionBuffer = 0;
	uint32_t mNormalBuffer = 0;
	uint32_t mUVBuffer = 0;
	uint32_t mIndexBuffer = 0;
	size_t mNumIndices = 0;
};

} // namespace gl
#endif