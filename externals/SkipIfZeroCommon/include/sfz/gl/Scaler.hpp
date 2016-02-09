#pragma once
#ifndef SFZ_GL_SCALER_HPP
#define SFZ_GL_SCALER_HPP

#include <cstdint>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/gl/PostProcessQuad.hpp"
#include "sfz/gl/Program.hpp"
#include "sfz/math/Vector.hpp"

namespace gl {

using sfz::AABB2D;
using sfz::vec2;
using std::int32_t;
using std::uint32_t;

// Scaling algorithm enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

enum class ScalingAlgorithm : int32_t {
	NEAREST = 0,
	BILINEAR = 1,
	GRID_2X2_NEAREST = 2,
	GRID_2X2_BILINEAR = 3,
	GRID_4X4_NEAREST = 4,
	GRID_4X4_BILINEAR = 5,
	BICUBIC_BSPLINE = 6,
	LANCZOS_2 = 7,
	LANCZOS_3 = 8
};

const char* to_string(ScalingAlgorithm algorithm) noexcept;

// Scaler class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Scaler final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Scaler(const Scaler&) = delete;
	Scaler& operator= (const Scaler&) = delete;

	Scaler(ScalingAlgorithm scalingAlgo = ScalingAlgorithm::NEAREST) noexcept;
	Scaler(Scaler&& other) noexcept;
	Scaler& operator= (Scaler&& other) noexcept;
	~Scaler() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void scale(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex, vec2 srcDimensions) noexcept;
	
	void scale(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex, vec2 srcDimensions) noexcept;

	void changeScalingAlgorithm(ScalingAlgorithm newAlgo) noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline ScalingAlgorithm scalingAlgorithm() const noexcept { return mScalingAlgorithm; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	PostProcessQuad mQuad;
	ScalingAlgorithm mScalingAlgorithm;
	Program mProgram;
	uint32_t mSamplerObject = 0;
};

} // namespace gl
#endif