#include "rendering/NewRenderer.hpp"

#include <sfz/gl/OpenGL.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/util/IO.hpp>

#include "rendering/Assets.hpp"

namespace s3 {

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static gl::Program compileStandardShaderProgram() noexcept
{
	return gl::Program::fromSource(R"(
		#version 330

		in vec3 position;
		in vec2 texCoordIn;

		out vec2 texCoord;

		uniform mat4 modelViewProj;

		void main()
		{
			gl_Position = modelViewProj * vec4(position, 1);
			texCoord = texCoordIn;
		}
	)", R"(
		#version 330

		precision highp float; // required by GLSL spec Sect 4.5.3

		in vec2 texCoord;
		uniform sampler2D tex;
		
		out vec4 fragmentColor;

		void main()
		{
			fragmentColor = vec4(1.0, 0.0, 0.0, 1.0);//texture(tex, texCoord.xy);
		}
	)", [](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "position");
		glBindAttribLocation(shaderProgram, 1, "texCoordIn");
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");
	});
}

static gl::Model& getTileModel(SnakeTile* tilePtr, float progress, bool gameOver) noexcept
{
	Assets& assets = Assets::INSTANCE();
	if (tilePtr->type() == TileType::HEAD) return assets.HEAD_D2U_F2_MODEL;
	return assets.BODY_D2U_MODEL;
	
	/*
	bool isTurn = s3::isTurn(tilePtr->from(), tilePtr->to());

	switch (tilePtr->type()) {
	case s3::TileType::EMPTY: return assets.TILE_FACE;
	case s3::TileType::OBJECT: return assets.OBJECT;
	case s3::TileType::BONUS_OBJECT: return assets.BONUS_OBJECT;

	case s3::TileType::HEAD:
		if (progress <= 0.5f) { // Frame 1
			return assets.HEAD_D2U_F1;
		} else { // Frame 2
			return assets.HEAD_D2U_F2;
		}
	case s3::TileType::PRE_HEAD:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return !gameOver ? assets.PRE_HEAD_D2U_F1 : assets.DEAD_PRE_HEAD_D2U_F1;
			else return !gameOver ? assets.PRE_HEAD_D2R_F1 : assets.DEAD_PRE_HEAD_D2R_F1;
		} else { // Frame 2
			if (!isTurn) return assets.BODY_D2U;
			else return assets.BODY_D2R;
		}
	case s3::TileType::BODY:
		if (!isTurn) return assets.BODY_D2U;
		else return assets.BODY_D2R;
	case s3::TileType::TAIL:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return assets.TAIL_D2U_F1;
			else return assets.TAIL_D2R_F1;
		} else { // Frame 2
			if (!isTurn) return assets.TAIL_D2U_F2;
			else return assets.TAIL_D2R_F2;
		}

	case s3::TileType::HEAD_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			return assets.HEAD_D2U_F1;
		} else { // Frame 2
			return assets.HEAD_D2U_F2;
		}
	case s3::TileType::PRE_HEAD_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return !gameOver ? assets.PRE_HEAD_D2U_DIG_F1 : assets.DEAD_PRE_HEAD_D2U_DIG_F1;
			else return !gameOver ? assets.PRE_HEAD_D2R_DIG_F1 : assets.DEAD_PRE_HEAD_D2R_DIG_F1;
		} else { // Frame 2
			if (!isTurn) return assets.BODY_D2U_DIG;
			else return assets.BODY_D2R_DIG;
		}
	case s3::TileType::BODY_DIGESTING:
		if (!isTurn) return assets.BODY_D2U_DIG;
		else return assets.BODY_D2R_DIG;
	case s3::TileType::TAIL_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return assets.TAIL_D2U_DIG_F1;
			else return assets.TAIL_D2R_DIG_F1;
		} else { // Frame 2
			if (!isTurn) return assets.TAIL_D2U_DIG_F2;
			else return assets.TAIL_D2R_DIG_F2;
		}
	}*/
}

static float getTileAngleRad(Direction3D side, Direction2D from) noexcept
{
	float angle;

	switch (from) {
	case Direction2D::UP:
		angle = 180.0f;
		break;
	case Direction2D::DOWN:
		angle = 0.0f;
		break;
	case Direction2D::LEFT:
		angle = -90.0f;
		break;
	case Direction2D::RIGHT:
		angle = 90.0f;
		break;
	}

	// Yeah, I dunno. There probably is a pattern here to make it general, but I don't see it.
	switch (side) {
	case Direction3D::NORTH:
		angle += 180.0f;
		break;
	case Direction3D::SOUTH:
		// Do nothing.
		break;
	case Direction3D::WEST:
		angle -= 90.0f;
		break;
	case Direction3D::EAST:
		angle += 90.0f;
		break;
	case Direction3D::UP:
		angle += 180.0f;
		break;
	case Direction3D::DOWN:
		// Do nothing.
		break;
	}

	return angle * sfz::DEG_TO_RAD();
}

static vec3 tilePosToVector(const Model& model, const Position& tilePos) noexcept
{
	// +0.5f to get the midpoint of the tile
	const float e1f = static_cast<float>(tilePos.e1) + 0.5f;
	const float e2f = static_cast<float>(tilePos.e2) + 0.5f;
	const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);

	return (e1f * tileWidth - 0.5f) * directionVector(tilePos.side, Coordinate::e1) +
		(e2f * tileWidth - 0.5f) * directionVector(tilePos.side, Coordinate::e2) +
		toVector(tilePos.side) * 0.5f;
}

static mat4 tileSpaceRotation(Direction3D side) noexcept
{
	switch (side) {
	case Direction3D::UP: return sfz::identityMatrix4<float>();
	case Direction3D::DOWN: return sfz::xRotationMatrix4(sfz::PI());
	case Direction3D::SOUTH: return sfz::xRotationMatrix4(sfz::PI()/2.0f);
	case Direction3D::NORTH: return sfz::xRotationMatrix4(-sfz::PI()/2.0f);
	case Direction3D::WEST: return sfz::zRotationMatrix4(sfz::PI()/2.0f);
	case Direction3D::EAST: return sfz::zRotationMatrix4(-sfz::PI()/2.0f);
	}
}

// NewRenderer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

NewRenderer::NewRenderer() noexcept
:
	mProgram{s3::compileStandardShaderProgram()},
	mTile{false, false},
	mXFlippedTile{true, false}
{ }

// NewRenderer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void NewRenderer::render(const Model& model, const Camera& cam, const AABB2D& viewport) noexcept
{
	Assets& assets = Assets::INSTANCE();

	float aspect = viewport.width() / viewport.height();
	mProjMatrix = sfz::glPerspectiveProjectionMatrix(cam.mFov, aspect, 0.1f, 50.0f);

	//glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// Clearing screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable culling
	glDisable(GL_CULL_FACE);

	glViewport(viewport.min.x, viewport.min.y, viewport.width(), viewport.height());

	glUseProgram(mProgram.handle());




	/*const mat4 viewProj = mProjMatrix * cam.mViewMatrix * sfz::scalingMatrix4(0.25f);
	gl::setUniform(mProgram, "modelViewProj", viewProj);
	mHeadModel.render();*/




	const mat4 viewProj = mProjMatrix * cam.mViewMatrix;

	// Only one texture is used when rendering SnakeTiles
	gl::setUniform(mProgram, "tex", 0);
	glActiveTexture(GL_TEXTURE0);

	// Render all SnakeTiles
	const size_t tilesPerSide = model.mCfg.gridWidth*model.mCfg.gridWidth;
	const float gridWidth = static_cast<float>(model.mCfg.gridWidth);
	const float tileWidth = 1.0f / gridWidth;
	const mat4 tileScaling = sfz::scalingMatrix4(tileWidth/16.0f);
	mat4 transform, tileSpaceRot, tileSpaceRotScaling;
	vec3 snakeFloatVec;
	SnakeTile *sidePtr, *tilePtr;
	Position tilePos;
	Direction3D currentSide;

	for (size_t side = 0; side < 6; side++) {
		currentSide = cam.mSideRenderOrder[side];
		sidePtr = model.getTilePtr(Position{currentSide, 0, 0});

		tileSpaceRot = tileSpaceRotation(currentSide);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = toVector(currentSide) * 0.001f;

		if (cam.mRenderTileFaceFirst[side]) {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = model.getTilePosition(tilePtr);

				// Calculate base transform
				transform = tileSpaceRotScaling;
				transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render tile face
				sfz::translation(transform, tilePosToVector(model, tilePos));
				gl::setUniform(mProgram, "modelViewProj", viewProj * transform);
				//glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.handle());
				//mTile.render();

				// Render snake sprite for non-empty tiles
				if (tilePtr->type() == s3::TileType::EMPTY) continue;

				// Tile Sprite Transform
				sfz::translation(transform, translation(transform) + snakeFloatVec);
				gl::setUniform(mProgram, "modelViewProj", viewProj * transform);
				//glBindTexture(GL_TEXTURE_2D, getTileTexture(tilePtr, model.mProgress, model.mGameOver).handle());
				//if (isLeftTurn(tilePtr->from(), tilePtr->to())) mXFlippedTile.render();
				//else mTile.render();
				getTileModel(tilePtr, model.mProgress, model.mGameOver).render();
			}
		} else {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = model.getTilePosition(tilePtr);

				// Calculate base transform
				transform = tileSpaceRotScaling;
				transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render snake sprite for non-empty tiles
				if (tilePtr->type() != s3::TileType::EMPTY) {
					sfz::translation(transform, tilePosToVector(model, tilePos) + snakeFloatVec);
					gl::setUniform(mProgram, "modelViewProj", viewProj * transform);
					//glBindTexture(GL_TEXTURE_2D, getTileTexture(tilePtr, model.mProgress, model.mGameOver).handle());
					//if (isLeftTurn(tilePtr->from(), tilePtr->to())) mXFlippedTile.render();
					//else mTile.render();
					getTileModel(tilePtr, model.mProgress, model.mGameOver).render();
				}

				// Render tile face
				//sfz::translation(transform, tilePosToVector(model, tilePos));
				//gl::setUniform(mProgram, "modelViewProj", viewProj * transform);
				//glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.handle());
				//mTile.render();
			}
		}
	}

	// Hack to correctly render dead snake head
	if (model.mGameOver) {
		SnakeTile* deadHeadPtr = model.mDeadHeadPtr;
		Position deadHeadPos = model.mDeadHeadPos;

		// Calculate dead head transform
		tileSpaceRot = tileSpaceRotation(deadHeadPos.side);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = toVector(deadHeadPos.side) * 0.0015f;
		transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(deadHeadPos.side, deadHeadPtr->from()));
		translation(transform, tilePosToVector(model, deadHeadPos) + snakeFloatVec);

		// Render dead head
		gl::setUniform(mProgram, "modelViewProj", viewProj * transform);
		/*glBindTexture(GL_TEXTURE_2D,
			getTileTexture(deadHeadPtr, model.mProgress, model.mGameOver).handle());
		if (isLeftTurn(deadHeadPtr->from(), deadHeadPtr->to())) mXFlippedTile.render();
		else mTile.render();*/
	}

	gl::FontRenderer& font = assets.fontRenderer;

	font.verticalAlign(gl::VerticalAlign::TOP);
	font.horizontalAlign(gl::HorizontalAlign::LEFT);

	font.begin(viewport); // TODO: Should not use viewport

	font.write(vec2{0.0f, (float)viewport.height()}, 64.0f, "Score: " + std::to_string(model.mScore));

	font.end(0, viewport.dimensions(), vec4{1.0f, 1.0f, 1.0f, 1.0f});

	if (model.mGameOver) {
		font.verticalAlign(gl::VerticalAlign::MIDDLE);
		font.horizontalAlign(gl::HorizontalAlign::CENTER);

		font.begin(viewport);

		font.write(viewport.position(), 160.0f, "Game Over");

		font.end(0, viewport.dimensions(), sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});
	}

	// Clean up
	glUseProgram(0);
}

} // namespace s3