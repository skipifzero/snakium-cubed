#pragma once
#ifndef SFZ_GL_SSAO_HPP
#define SFZ_GL_SSAO_HPP

#include <cstdint>
#include <memory>

#include <sfz/gl/Framebuffer.hpp>
#include <sfz/gl/PostProcessQuad.hpp>
#include <sfz/gl/Program.hpp>
#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>

namespace gl {

using sfz::vec2;
using sfz::vec3;
using sfz::vec2i;
using sfz::mat4;

using std::int32_t;
using std::size_t;
using std::uint32_t;
using std::unique_ptr;

/**
 * @brief Class used for calculating screen space ambient occlusion
 */
class SSAO final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SSAO(const SSAO&) noexcept = default; // Default constructor will not create a usable instance
	SSAO& operator= (const SSAO&) = delete;
	SSAO(SSAO&&) noexcept = default;
	SSAO& operator= (SSAO&&) noexcept = default;
	
	/**
	 * @brief Creates a new SSAO instance
	 * @param dimensions the size of the SSAO texture
	 * @param numSamples the amount of SSAO samples to take, max 128
	 * @param radius the radius to sample around each point
	 * @param minDepthBias the minimum depth difference needed for an occluder to register
	 * @param occlusionPower a factor to scale the strength of the ssao
	 * @param blurOcclusion whether to blur the ssao or not 
	 */
	SSAO(vec2i dimensions, size_t numSamples, float radius, float minDepthBias = 0.001f,
	     float occlusionPower = 1.0f, bool blurOcclusion = true) noexcept;
	
	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/**
	 * @brief Calculates SSAO
	 * @param linearDepthTex the linear depth texture (i.e. depth = -vsPos.z / uFarPlaneDist)
	 * @param normalTex the normal texture
	 * @param projMatrix the projection matrix
	 * @param farPlaneDist the distance to the far plane
	 * @return texture handle to the internal texture holding the result
	 */
	uint32_t calculate(uint32_t linearDepthTex, uint32_t normalTex,
	                   const mat4& projMatrix, float farPlaneDist) noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline vec2i dimensions() const noexcept { return mDimensions; }
	inline int32_t width() const noexcept { return mDimensions.x; }
	inline int32_t height() const noexcept { return mDimensions.y; }
	inline size_t numSamples() const noexcept { return mKernelSize; }
	inline float radius() const noexcept { return mRadius; }
	inline float minDepthBias() const noexcept { return mMinDepthBias; }
	inline float occlusionPower() const noexcept { return mOcclusionPower; }
	inline bool blurOcclusion() const noexcept { return mBlurOcclusion; }

	// Setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void dimensions(vec2i dim) noexcept;
	void dimensions(int width, int height) noexcept;
	void numSamples(size_t numSamples) noexcept;
	void radius(float radius) noexcept;
	void minDepthBias(float minDepthBias) noexcept;
	void occlusionPower(float occlusionPower) noexcept;
	void blurOcclusion(bool blurOcclusion) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Program mSSAOProgram, mHorizontalBlurProgram, mVerticalBlurProgram;
	PostProcessQuad mPostProcessQuad;
	Framebuffer mOcclusionFBO, mTempFBO;

	vec2i mDimensions;
	float mRadius, mMinDepthBias, mOcclusionPower;
	bool mBlurOcclusion;
	size_t mKernelSize;
	unique_ptr<vec3[]> mKernel;
	unique_ptr<vec3[]> mNoise;
};

} // namespace gl
#endif
