#pragma once
#ifndef SFZ_RENDERING_MODERN_RENDERER_HPP
#define SFZ_RENDERING_MODERN_RENDERER_HPP

#include <vector>

#include <sfz/gl/BoxBlur.hpp>
#include <sfz/gl/Program.hpp>
#include <sfz/gl/PostProcessFB.hpp>
#include <sfz/gl/Scaler.hpp>
#include <sfz/gl/ShadowMapFB.hpp>
#include <sfz/geometry/AABB2D.hpp>
#include <sfz/math/Matrix.hpp>

#include "gamelogic/Model.hpp"
#include "rendering/Camera.hpp"
#include "rendering/Framebuffers.hpp"
#include "rendering/Spotlight.hpp"

namespace s3 {

using gl::PostProcessFB;
using gl::Program;
using sfz::AABB2D;
using sfz::mat4;
using sfz::vec2;
using std::vector;

class ModernRenderer final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ModernRenderer(const ModernRenderer&) = delete;
	ModernRenderer& operator= (const ModernRenderer&) = delete;

	ModernRenderer() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void render(const Model& model, const Camera& cam, vec2 drawableDim) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gl::PostProcessQuad mPostProcessQuad;
	Program mGBufferGenProgram, mEmissiveGenProgram, mShadowMapProgram,
	            mSpotlightShadingProgram, mLightShaftsProgram, mGlobalShadingProgram;
	gl::Scaler mScaler;
	gl::BoxBlur mBoxBlur;
	GBuffer mGBuffer;
	PostProcessFB mEmissiveFB, mSpotlightShadingFB, mLightShaftsFB, mGlobalShadingFB;
	vector<Spotlight> mSpotlights;
	gl::ShadowMapFB mShadowMapHighRes, mShadowMapLowRes;
};

} // namespace s3
#endif