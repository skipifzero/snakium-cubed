#pragma once
#ifndef SFZ_GL_SPRITE_BATCH_HPP
#define SFZ_GL_SPRITE_BATCH_HPP

#include <cstddef> // size_t
#include <cstdint>
#include <memory>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/gl/Program.hpp"
#include "sfz/gl/TextureRegion.hpp"
#include "sfz/math/Matrix.hpp"
#include "sfz/math/Vector.hpp"

namespace gl {

using sfz::AABB2D;
using sfz::vec2;
using sfz::vec4;
using sfz::mat3;

using std::int32_t;
using std::size_t;
using std::uint32_t;
using std::unique_ptr;

// SpriteBatch
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class SpriteBatch final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SpriteBatch() noexcept = delete;
	SpriteBatch(const SpriteBatch&) noexcept = delete;
	SpriteBatch& operator= (const SpriteBatch&) noexcept = delete;

	/**
	 * @brief Creates a SpriteBatch with the specified capacity.
	 */
	SpriteBatch(size_t capacity) noexcept;
	/**
	 * @brief Creates a SpriteBatch with a custom fragment shader.
	 * The fragment shader needs the following declarations:
	 * in vec2 uvCoord;
	 * out vec4 fragmentColor
	 * uniform sampler2D uTexture;
	 */
	SpriteBatch(size_t capacity, const char* fragmentShaderSrc) noexcept;
	SpriteBatch(SpriteBatch&& other) noexcept;
	SpriteBatch& operator= (SpriteBatch&& other) noexcept;
	~SpriteBatch() noexcept;

	// Public interface
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void begin(const AABB2D& camera) noexcept;
	void begin(vec2 cameraPosition, vec2 cameraDimensions) noexcept;

	void draw(const AABB2D& rect, const TextureRegion& texRegion) noexcept;
	void draw(vec2 position, vec2 dimensions, const TextureRegion& texRegion) noexcept;

	void draw(vec2 position, vec2 dimensions, float angleRads,
	          const TextureRegion& texRegion) noexcept;

	void end(uint32_t fbo, vec2 viewportDimensions, uint32_t texture) noexcept;
	/** viewport is in same coordinate system as glViewport() (i.e. (0,0) in lower left corner) */
	void end(uint32_t fbo, const AABB2D& viewport, uint32_t texture) noexcept;

	inline const gl::Program& shaderProgram() const noexcept { return mShader; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	size_t mCapacity;
	size_t mCurrentDrawCount;
	mat3 mCamProj;

	gl::Program mShader;
	int32_t mTextureUniformLoc = 0;
	uint32_t mVAO;
	uint32_t mVertexBuffer, mIndexBuffer, mTransformBuffer, mUVBuffer;
	unique_ptr<mat3[]> mTransformArray;
	unique_ptr<vec4[]> mUVArray;
};

} // namespace sfz
#endif