#pragma once
#ifndef SFZ_RENDERING_CLASSIC_RENDERER_HPP
#define SFZ_RENDERING_CLASSIC_RENDERER_HPP

#include <sfz/gl/Program.hpp>
#include <sfz/geometry/AABB2D.hpp>
#include <sfz/math/Matrix.hpp>

#include "gamelogic/Model.hpp"
#include "rendering/Camera.hpp"
#include "rendering/TileObject.hpp"

namespace s3 {

using sfz::AABB2D;
using sfz::mat4;

class ClassicRenderer final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ClassicRenderer(const ClassicRenderer&) = delete;
	ClassicRenderer& operator= (const ClassicRenderer&) = delete;

	ClassicRenderer() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void render(const Model& model, const Camera& cam, const AABB2D& viewport) noexcept;

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gl::Program mProgram;
	TileObject mTile, mXFlippedTile;
	mat4 mProjMatrix;
};

} // namespace s3
#endif