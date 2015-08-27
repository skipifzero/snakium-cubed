#include "screens/GameScreen.hpp"

#include <sfz/GL.hpp>

#include "Assets.hpp"
#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "Rendering.hpp"
#include "screens/MainMenuScreen.hpp"

namespace s3 {

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

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

// GameScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GameScreen::GameScreen(sdl::Window& window, const ModelConfig& modelCfg) noexcept
:
	mWindow{window},
	mModel{modelCfg},
	mTile{false, false},
	mXFlippedTile{true, false}
{
	mShaderProgram = s3::compileStandardShaderProgram();

	float aspect = static_cast<float>(window.width()) / static_cast<float>(window.height());
	mProjMatrix = sfz::glPerspectiveProjectionMatrix(mCam.mFov, aspect, 0.1f, 50.0f);

	mIsTransparent = GlobalConfig::INSTANCE().transparentCube;
}

// GameScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp GameScreen::update(const UpdateState& state)
{
	const float delta = state.delta;
	GlobalConfig& cfg = GlobalConfig::INSTANCE();

	// Handle input
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
				          std::unique_ptr<sfz::BaseScreen>{new MainMenuScreen{mWindow}}};
			case SDLK_SPACE:
				mIsPaused = !mIsPaused;
				break;
			case SDLK_UP:
			case 'w':
			case 'W':
				mModel.changeDirection(mCam.mUpDir, Direction2D::UP);
				break;
			case SDLK_DOWN:
			case 's':
			case 'S':
				mModel.changeDirection(mCam.mUpDir, Direction2D::DOWN);
				break;
			case SDLK_LEFT:
			case 'a':
			case 'A':
				mModel.changeDirection(mCam.mUpDir, Direction2D::LEFT);
				break;
			case SDLK_RIGHT:
			case 'd':
			case 'D':
				mModel.changeDirection(mCam.mUpDir, Direction2D::RIGHT);
				break;
			case 'x':
			case 'X':
				mIsTransparent = !mIsTransparent;
				break;
			case 'z':
			case 'Z':
				mIsTransparent = true;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case 'z':
			case 'Z':
				mIsTransparent = cfg.transparentCube;
				break;
			}
			break;
		}
	}

	// Updating
	if (mIsPaused) return sfz::SCREEN_NO_OP;

	mModel.update(delta);
	if (!mModel.mGameOver) mCam.update(mModel, delta);

	return sfz::SCREEN_NO_OP;
}

void GameScreen::render(const UpdateState& state)
{
	Assets& assets = Assets::INSTANCE();

	//glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	
	// Clearing screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable/Disable culling
	if (mIsTransparent) glDisable(GL_CULL_FACE);
	else glEnable(GL_CULL_FACE);

	glViewport(0, 0, mWindow.drawableWidth(), mWindow.drawableHeight());

	glUseProgram(mShaderProgram);

	const mat4 viewProj = mProjMatrix * mCam.mViewMatrix;

	// Only one texture is used when rendering SnakeTiles
	gl::setUniform(mShaderProgram, "tex", 0);
	glActiveTexture(GL_TEXTURE0);

	// Render all SnakeTiles
	const size_t tilesPerSide = mModel.mCfg.gridWidth*mModel.mCfg.gridWidth;
	const float gridWidth = static_cast<float>(mModel.mCfg.gridWidth);
	const float tileWidth = 1.0f / gridWidth;
	const mat4 tileScaling = sfz::scalingMatrix4(tileWidth);
	mat4 transform, tileSpaceRot, tileSpaceRotScaling;
	vec3 snakeFloatVec;
	SnakeTile *sidePtr, *tilePtr;
	Position tilePos;
	Direction3D currentSide;

	for (size_t side = 0; side < 6; side++) {
		currentSide = mCam.mSideRenderOrder[side];
		sidePtr = mModel.getTilePtr(Position{currentSide, 0, 0});

		tileSpaceRot = tileSpaceRotation(currentSide);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = toVector(currentSide) * 0.001f;

		if (mCam.mRenderTileFaceFirst[side]) {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = mModel.getTilePosition(tilePtr);

				// Calculate base transform
				transform = tileSpaceRotScaling;
				transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render tile face
				translation(transform, tilePosToVector(mModel, tilePos));
				gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.mHandle);
				mTile.render();

				// Render snake sprite for non-empty tiles
				if(tilePtr->type() == s3::TileType::EMPTY) continue;

				// Tile Sprite Transform
				translation(transform, translation(transform) + snakeFloatVec);
				gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D,
				     assets.getTileTexture(tilePtr, mModel.mProgress, mModel.mGameOver).mHandle);
				if (isLeftTurn(tilePtr->from(), tilePtr->to())) mXFlippedTile.render();
				else mTile.render();
			}
		} else {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = mModel.getTilePosition(tilePtr);

				// Calculate base transform
				transform = tileSpaceRotScaling;
				transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render snake sprite for non-empty tiles
				if(tilePtr->type() != s3::TileType::EMPTY) {
					translation(transform, tilePosToVector(mModel, tilePos) + snakeFloatVec);
					gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
					glBindTexture(GL_TEXTURE_2D,
					     assets.getTileTexture(tilePtr, mModel.mProgress, mModel.mGameOver).mHandle);
					if (isLeftTurn(tilePtr->from(), tilePtr->to())) mXFlippedTile.render();
					else mTile.render();
				}

				// Render tile face
				translation(transform, tilePosToVector(mModel, tilePos));
				gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.mHandle);
				mTile.render();
			}
		}
	}

	// Hack to correctly render dead snake head
	if (mModel.mGameOver) {
		SnakeTile* deadHeadPtr = mModel.mDeadHeadPtr;
		Position deadHeadPos = mModel.mDeadHeadPos;

		// Calculate dead head transform
		tileSpaceRot = tileSpaceRotation(deadHeadPos.side);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = toVector(deadHeadPos.side) * 0.0015f;
		transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(deadHeadPos.side, deadHeadPtr->from()));
		translation(transform, tilePosToVector(mModel, deadHeadPos) + snakeFloatVec);

		// Render dead head
		gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
		glBindTexture(GL_TEXTURE_2D,
		   assets.getTileTexture(deadHeadPtr, mModel.mProgress, mModel.mGameOver).mHandle);
		if (isLeftTurn(deadHeadPtr->from(), deadHeadPtr->to())) mXFlippedTile.render();
		else mTile.render();
	}

	gl::FontRenderer& font = assets.fontRenderer;

	font.verticalAlign(gl::VerticalAlign::TOP);
	font.horizontalAlign(gl::HorizontalAlign::LEFT);

	font.begin(mWindow.drawableDimensions()/2.0f, mWindow.drawableDimensions());

	font.write(sfz::vec2{0.0f, (float)mWindow.drawableHeight()}, 64.0f, "Score: " + std::to_string(mModel.mScore));

	font.end(0, mWindow.drawableDimensions(), sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});

	if (mModel.mGameOver) {
		font.verticalAlign(gl::VerticalAlign::MIDDLE);
		font.horizontalAlign(gl::HorizontalAlign::CENTER);

		font.begin(mWindow.drawableDimensions()/2.0f, mWindow.drawableDimensions());

		font.write(mWindow.drawableDimensions()/2.0f, 160.0f, "Game Over");

		font.end(0, mWindow.drawableDimensions(), sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});
	}

	// Clean up
	glUseProgram(0);
}

void GameScreen::onResize(vec2 dimensions, vec2 drawableDimensions)
{
	mProjMatrix = sfz::glPerspectiveProjectionMatrix(mCam.mFov, dimensions.x/dimensions.y, 0.1f, 50.0f);
}

} // namespace s3