#include "screens/GameScreen.hpp"

#include "GameLogic.hpp"
#include "Rendering.hpp"

namespace s3 {

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace {

void checkGLErrorsMessage(const std::string& msg) noexcept
{
	if (gl::checkAllGLErrors()) std::cerr << msg << std::endl;
}

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

GameScreen::GameScreen(sdl::Window& window, s3::Assets& assets, GlobalConfig& cfg) noexcept
:
	mCfg{cfg},
	mWindow{window},
	mAssets{assets},
	mModel{cfg.mModelConfig}
{
	shaderProgram = s3::compileStandardShaderProgram();

	float aspect = static_cast<float>(window.width()) / static_cast<float>(window.height());
	projMatrix = sfz::glPerspectiveProjectionMatrix(cam.mFov, aspect, 0.1f, 50.0f);

	isTransparent = mCfg.mTransparentCube;
}

// GameScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void GameScreen::update(const std::vector<SDL_Event>& events, float delta)
{
	for (const SDL_Event& event : events) {
		mQuit = handleInput(mModel, event);
		if (mQuit) return;
	}
	mQuit = update(mModel, delta);
}

IScreen* GameScreen::changeScreen()
{
	return nullptr;
}

bool GameScreen::quit()
{
	return mQuit;
}

void GameScreen::render(float delta)
{
	render(mWindow, mAssets, mModel, delta);
}

// GameScreen: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool GameScreen::handleInput(s3::Model& model, const SDL_Event& event)
{
	switch (event.type) {
	case SDL_QUIT: return true;
	case SDL_WINDOWEVENT:
		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			float w = static_cast<float>(event.window.data1);
			float h = static_cast<float>(event.window.data2);
			projMatrix = sfz::glPerspectiveProjectionMatrix(cam.mFov, w/h, 0.1f, 50.0f);
		}
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE: return true;
		case SDLK_SPACE:
			isPaused = !isPaused;
			break;
		case SDLK_UP:
			model.changeDirection(cam.mUpDir, s3::Direction2D::UP);
			break;
		case SDLK_DOWN:
			model.changeDirection(cam.mUpDir, s3::Direction2D::DOWN);
			break;
		case SDLK_LEFT:
			model.changeDirection(cam.mUpDir, s3::Direction2D::LEFT);
			break;
		case SDLK_RIGHT:
			model.changeDirection(cam.mUpDir, s3::Direction2D::RIGHT);
			break;
		case 'w':
		case 'W':
			model.changeDirection(cam.mUpDir, s3::Direction2D::UP);
			break;
		case 's':
		case 'S':
			model.changeDirection(cam.mUpDir, s3::Direction2D::DOWN);
			break;
		case 'a':
		case 'A':
			model.changeDirection(cam.mUpDir, s3::Direction2D::LEFT);
			break;
		case 'd':
		case 'D':
			model.changeDirection(cam.mUpDir, s3::Direction2D::RIGHT);
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
			isTransparent = mCfg.mTransparentCube;
			break;
		}
		break;
	}
	return false;
}

bool GameScreen::update(s3::Model& model, float delta)
{
	if (isPaused) return false;

	model.update(delta);
	if (!model.mGameOver) cam.update(model, delta);

	return false;
}

void GameScreen::render(sdl::Window& window, s3::Assets& assets, s3::Model& model, float)
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

	glViewport(0, 0, window.drawableWidth(), window.drawableHeight());

	glUseProgram(shaderProgram);

	const sfz::mat4 viewProj = projMatrix * cam.mViewMatrix;

	// Only one texture is used when rendering SnakeTiles
	gl::setUniform(shaderProgram, "tex", 0);
	glActiveTexture(GL_TEXTURE0);

	// Render all SnakeTiles
	const size_t tilesPerSide = model.mCfg.gridWidth*model.mCfg.gridWidth;
	const float gridWidth = static_cast<float>(model.mCfg.gridWidth);
	const float tileWidth = 1.0f / gridWidth;
	const sfz::mat4 tileScaling = sfz::scalingMatrix4(tileWidth);
	sfz::mat4 transform, tileSpaceRot, tileSpaceRotScaling;
	sfz::vec3 snakeFloatVec;
	s3::SnakeTile *sidePtr, *tilePtr;
	s3::Position tilePos;
	s3::Direction3D currentSide;

	for (size_t side = 0; side < 6; side++) {
		currentSide = cam.mSideRenderOrder[side];
		sidePtr = model.getTilePtr(s3::Position{currentSide, 0, 0});

		tileSpaceRot = tileSpaceRotation(currentSide);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = s3::toVector(currentSide) * 0.001f;

		if (cam.mRenderTileFaceFirst[side]) {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = model.getTilePosition(tilePtr);

				// Calculate base transform
				transform = tileSpaceRotScaling;
				transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render tile face
				translation(transform, tilePosToVector(model, tilePos));
				gl::setUniform(shaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.mHandle);
				tile.render();

				// Render snake sprite for non-empty tiles
				if(tilePtr->type() == s3::TileType::EMPTY) continue;

				// Tile Sprite Transform
				translation(transform, translation(transform) + snakeFloatVec);
				gl::setUniform(shaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D,
				     assets.getTileTexture(tilePtr, model.mProgress, model.mGameOver).mHandle);
				if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
				else tile.render();
			}
		} else {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = model.getTilePosition(tilePtr);

				// Calculate base transform
				transform = tileSpaceRotScaling;
				transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render snake sprite for non-empty tiles
				if(tilePtr->type() != s3::TileType::EMPTY) {
					translation(transform, tilePosToVector(model, tilePos) + snakeFloatVec);
					gl::setUniform(shaderProgram, "modelViewProj", viewProj * transform);
					glBindTexture(GL_TEXTURE_2D,
					     assets.getTileTexture(tilePtr, model.mProgress, model.mGameOver).mHandle);
					if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
					else tile.render();
				}

				// Render tile face
				translation(transform, tilePosToVector(model, tilePos));
				gl::setUniform(shaderProgram, "modelViewProj", viewProj * transform);
				glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.mHandle);
				tile.render();
			}
		}
	}

	// Hack to correctly render dead snake head
	if (model.mGameOver) {
		s3::SnakeTile* deadHeadPtr = model.mDeadHeadPtr;
		s3::Position deadHeadPos = model.mDeadHeadPos;

		// Calculate dead head transform
		tileSpaceRot = tileSpaceRotation(deadHeadPos.side);
		tileSpaceRotScaling = tileSpaceRot * tileScaling;
		snakeFloatVec = s3::toVector(deadHeadPos.side) * 0.0015f;
		transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(deadHeadPos.side, deadHeadPtr->from()));
		translation(transform, tilePosToVector(model, deadHeadPos) + snakeFloatVec);

		// Render dead head
		gl::setUniform(shaderProgram, "modelViewProj", viewProj * transform);
		glBindTexture(GL_TEXTURE_2D,
		   assets.getTileTexture(deadHeadPtr, model.mProgress, model.mGameOver).mHandle);
		if (isLeftTurn(deadHeadPtr->from(), deadHeadPtr->to())) xFlippedTile.render();
		else tile.render();
	}

	gl::FontRenderer& font = assets.mFontRenderer;

	font.verticalAlign(gl::VerticalAlign::TOP);
	font.horizontalAlign(gl::HorizontalAlign::LEFT);

	font.begin(window.drawableDimensions()/2.0f, window.drawableDimensions());

	font.write(sfz::vec2{0.0f, (float)window.drawableHeight()}, 64.0f, "Score: " + std::to_string(model.mScore));

	font.end(0, window.drawableDimensions(), sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});

	if (model.mGameOver) {
		font.verticalAlign(gl::VerticalAlign::MIDDLE);
		font.horizontalAlign(gl::HorizontalAlign::CENTER);

		font.begin(window.drawableDimensions()/2.0f, window.drawableDimensions());

		font.write(window.drawableDimensions()/2.0f, 160.0f, "Game Over");

		font.end(0, window.drawableDimensions(), sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});
	}

	// Clean up
	glUseProgram(0);
}

} // namespace s3