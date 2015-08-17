#include "screens/GameScreen.hpp"

#include "GameLogic.hpp"
#include "Rendering.hpp"

namespace s3 {

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace {

float getTileAngleRad(s3::Direction3D side, s3::Direction2D from) noexcept
{
	float angle;

	switch (from) {
	case s3::Direction2D::UP:
		angle = 180.0f;
		break;
	case s3::Direction2D::DOWN:
		angle = 0.0f;
		break;
	case s3::Direction2D::LEFT:
		angle = -90.0f;
		break;
	case s3::Direction2D::RIGHT:
		angle = 90.0f;
		break;
	}

	// Yeah, I dunno. There probably is a pattern here to make it general, but I don't see it.
	switch (side) {
	case s3::Direction3D::NORTH:
		angle += 180.0f;
		break;
	case s3::Direction3D::SOUTH:
		// Do nothing.
		break;
	case s3::Direction3D::WEST:
		angle -= 90.0f;
		break;
	case s3::Direction3D::EAST:
		angle += 90.0f;
		break;
	case s3::Direction3D::UP:
		angle += 180.0f;
		break;
	case s3::Direction3D::DOWN:
		// Do nothing.
		break;
	}

	return angle * sfz::DEG_TO_RAD();
}

sfz::vec3 tilePosToVector(const s3::Model& model, const s3::Position& tilePos) noexcept
{
	// +0.5f to get the midpoint of the tile
	const float e1f = static_cast<float>(tilePos.e1) + 0.5f;
	const float e2f = static_cast<float>(tilePos.e2) + 0.5f; 
	const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);

	return (e1f * tileWidth - 0.5f) * s3::directionVector(tilePos.side, s3::Coordinate::e1) +
	       (e2f * tileWidth - 0.5f) * s3::directionVector(tilePos.side, s3::Coordinate::e2) +
	       s3::toVector(tilePos.side) * 0.5f;
}

sfz::mat4 tileSpaceRotation(s3::Direction3D side) noexcept
{
	switch (side) {
	case s3::Direction3D::UP: return sfz::identityMatrix4<float>();
	case s3::Direction3D::DOWN: return sfz::xRotationMatrix4(sfz::PI());
	case s3::Direction3D::SOUTH: return sfz::xRotationMatrix4(sfz::PI()/2.0f);
	case s3::Direction3D::NORTH: return sfz::xRotationMatrix4(-sfz::PI()/2.0f);
	case s3::Direction3D::WEST: return sfz::zRotationMatrix4(sfz::PI()/2.0f);
	case s3::Direction3D::EAST: return sfz::zRotationMatrix4(-sfz::PI()/2.0f);
	}
}

} // anonymous namespace

// GameScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GameScreen::GameScreen(sdl::Window& window, s3::Assets& assets, const ModelConfig& modelCfg) noexcept
:
	mWindow{window},
	mAssets{assets},
	mModel{modelCfg}
{
	mShaderProgram = s3::compileStandardShaderProgram();

	float aspect = static_cast<float>(window.width()) / static_cast<float>(window.height());
	projMatrix = sfz::glPerspectiveProjectionMatrix(mCam.mFov, aspect, 0.1f, 50.0f);

	isTransparent = mCfg.transparentCube;
}

// GameScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScreenUpdateOp GameScreen::update(const vector<SDL_Event>& events,
	                              const unordered_map<int32_t, sdl::GameController>& controllers,
	                              float delta)
{
	// Handle input
	for (const SDL_Event& event : events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return sfz::ScreenUpdateOp{sfz::ScreenUpdateOpType::SWITCH_SCREEN,
				          std::unique_ptr<sfz::BaseScreen>{new MainMenuScreen{mWindow, mAssets}}};
			case SDLK_SPACE:
				isPaused = !isPaused;
				break;
			case SDLK_UP:
			case 'w':
			case 'W':
				mModel.changeDirection(mCam.mUpDir, s3::Direction2D::UP);
				break;
			case SDLK_DOWN:
			case 's':
			case 'S':
				mModel.changeDirection(mCam.mUpDir, s3::Direction2D::DOWN);
				break;
			case SDLK_LEFT:
			case 'a':
			case 'A':
				mModel.changeDirection(mCam.mUpDir, s3::Direction2D::LEFT);
				break;
			case SDLK_RIGHT:
			case 'd':
			case 'D':
				mModel.changeDirection(mCam.mUpDir, s3::Direction2D::RIGHT);
				break;
			case 'x':
			case 'X':
				isTransparent = !isTransparent;
				break;
			case 'z':
			case 'Z':
				isTransparent = true;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case 'z':
			case 'Z':
				isTransparent = mCfg.transparentCube;
				break;
			}
			break;
		}
	}

	// Updating
	if (isPaused) return sfz::SCREEN_NO_OP;

	mModel.update(delta);
	if (!mModel.mGameOver) mCam.update(mModel, delta);

	return sfz::SCREEN_NO_OP;
}

void GameScreen::render(float delta)
{
	static s3::TileObject tile{false, false};
	static s3::TileObject xFlippedTile{true, false};

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
	if (isTransparent) glDisable(GL_CULL_FACE);
	else glEnable(GL_CULL_FACE);

	glViewport(0, 0, mWindow.drawableWidth(), mWindow.drawableHeight());

	glUseProgram(mShaderProgram);

	const sfz::mat4 viewProj = projMatrix * mCam.mViewMatrix;

	// Only one texture is used when rendering SnakeTiles
	gl::setUniform(mShaderProgram, "tex", 0);
	glActiveTexture(GL_TEXTURE0);

	// Render all SnakeTiles
	const size_t tilesPerSide = mModel.mCfg.gridWidth*mModel.mCfg.gridWidth;
	const float gridWidth = static_cast<float>(mModel.mCfg.gridWidth);
	const float tileWidth = 1.0f / gridWidth;
	const sfz::mat4 tileScaling = sfz::scalingMatrix4(tileWidth);
	sfz::mat4 transform, tileSpaceRot, tileSpaceRotScaling;
	sfz::vec3 snakeFloatVec;
	s3::SnakeTile *sidePtr, *tilePtr;
	s3::Position tilePos;
	s3::Direction3D currentSide;

	for (size_t side = 0; side < 6; side++) {
		currentSide = mCam.mSideRenderOrder[side];
		sidePtr = mModel.getTilePtr(s3::Position{currentSide, 0, 0});

		tileSpaceRot = tileSpaceRotation(currentSide);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = s3::toVector(currentSide) * 0.001f;

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
				glBindTexture(GL_TEXTURE_2D, mAssets.TILE_FACE.mHandle);
				tile.render();

				// Render snake sprite for non-empty tiles
				if(tilePtr->type() == s3::TileType::EMPTY) continue;

				// Tile Sprite Transform
				translation(transform, translation(transform) + snakeFloatVec);
				gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D,
				     mAssets.getTileTexture(tilePtr, mModel.mProgress, mModel.mGameOver).mHandle);
				if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
				else tile.render();
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
					     mAssets.getTileTexture(tilePtr, mModel.mProgress, mModel.mGameOver).mHandle);
					if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
					else tile.render();
				}

				// Render tile face
				translation(transform, tilePosToVector(mModel, tilePos));
				gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D, mAssets.TILE_FACE.mHandle);
				tile.render();
			}
		}
	}

	// Hack to correctly render dead snake head
	if (mModel.mGameOver) {
		s3::SnakeTile* deadHeadPtr = mModel.mDeadHeadPtr;
		s3::Position deadHeadPos = mModel.mDeadHeadPos;

		// Calculate dead head transform
		tileSpaceRot = tileSpaceRotation(deadHeadPos.side);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = s3::toVector(deadHeadPos.side) * 0.0015f;
		transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(deadHeadPos.side, deadHeadPtr->from()));
		translation(transform, tilePosToVector(mModel, deadHeadPos) + snakeFloatVec);

		// Render dead head
		gl::setUniform(mShaderProgram, "modelViewProj", viewProj * transform);
		glBindTexture(GL_TEXTURE_2D,
		   mAssets.getTileTexture(deadHeadPtr, mModel.mProgress, mModel.mGameOver).mHandle);
		if (isLeftTurn(deadHeadPtr->from(), deadHeadPtr->to())) xFlippedTile.render();
		else tile.render();
	}

	gl::FontRenderer& font = mAssets.mFontRenderer;

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

void GameScreen::onQuit()
{
	// Nothing currently needs to be done
}

void GameScreen::onResize(vec2 dimensions)
{
	projMatrix = sfz::glPerspectiveProjectionMatrix(mCam.mFov, dimensions.x/dimensions.y, 0.1f, 50.0f);
}

} // namespace s3