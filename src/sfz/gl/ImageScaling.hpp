#pragma once
#ifndef SFZ_GL_IMAGE_SCALING_HPP
#define SFZ_GL_IMAGE_SCALING_HPP

#include <cstdint>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/math/Vector.hpp"

namespace gl {

using sfz::AABB2D;
using sfz::vec2;
using std::uint32_t;

// Resampling Algorithm enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

enum class ResamplingAlgorithm {
	NEAREST
};

// Scaling functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void scaleCopy(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex,
               ResamplingAlgorithm resamplingAlgo) noexcept;
void scaleCopy(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex,
               ResamplingAlgorithm resamplingAlgo) noexcept;

void scaleCopyNearest(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex) noexcept;
void scaleCopyNearest(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex) noexcept;

} // namespace gl
#endif