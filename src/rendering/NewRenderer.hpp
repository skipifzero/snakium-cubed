#pragma once
#ifndef SFZ_RENDERING_NEW_RENDERER_HPP
#define SFZ_RENDERING_NEW_RENDERER_HPP

#include <sfz/gl/Program.hpp>
#include <sfz/geometry/AABB2D.hpp>
#include <sfz/math/Matrix.hpp>

#include "gamelogic/Model.hpp"
#include "rendering/Camera.hpp"

namespace s3 {

using sfz::AABB2D;
using sfz::mat4;

class NewRenderer final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	NewRenderer(const NewRenderer&) = delete;
	NewRenderer& operator= (const NewRenderer&) = delete;

	NewRenderer() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void render(const Model& model, const Camera& cam, const AABB2D& viewport) noexcept;

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gl::Program mProgram;
	mat4 mProjMatrix;
};

} // namespace s3
#endif