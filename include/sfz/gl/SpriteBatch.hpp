#pragma once
#ifndef SFZ_GL_SPRITE_BATCH_HPP
#define SFZ_GL_SPRITE_BATCH_HPP

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/TextureRegion.hpp"
#include "sfz/Math.hpp"

#include <cstddef> // size_t
#include <memory>

namespace gl {

using sfz::AABB2D;
using sfz::vec2;
using sfz::vec4;
using sfz::mat3;
using sfz::TextureRegion;
using std::size_t;

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

	void end(GLuint fbo, vec2 viewportDimensions, GLuint texture) noexcept;
	/** viewport is in same coordinate system as glViewport() (i.e. (0,0) in lower left corner) */
	void end(GLuint fbo, const AABB2D& viewport, GLuint texture) noexcept;

	inline GLuint shaderProgram() const noexcept { return mShader; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	size_t mCapacity;
	size_t mCurrentDrawCount;
	mat3 mCamProj;

	GLuint mShader;
	GLuint mVAO;
	GLuint mVertexBuffer, mIndexBuffer, mTransformBuffer, mUVBuffer;
	std::unique_ptr<mat3[]> mTransformArray;
	std::unique_ptr<vec4[]> mUVArray;
};

} // namespace sfz
#endif