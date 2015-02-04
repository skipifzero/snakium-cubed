#include <iostream>
#include <exception> // std::terminate()
#include <string>
#include <chrono>
#include <memory>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "Assets.hpp"
#include "Camera.hpp"
#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "S3Shaders.hpp"
#include "TileObject.hpp"

#undef main // Remove SDL hack until we can get it to compile properly

#include "sfz/MSVC12HackON.hpp"

// Variables
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
GLuint shaderProgram;
s3::GlobalConfig globalConfig;
s3::Camera cam;
sfz::mat4f projMatrix;

bool isTransparent = false;
bool isPaused = false;

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace {

float calculateDelta() noexcept
{
	static std::chrono::high_resolution_clock::time_point previousTime, currentTime;

	previousTime = currentTime;
	currentTime = std::chrono::high_resolution_clock::now();

	using FloatSecondDuration = std::chrono::duration<float>;
	return std::chrono::duration_cast<FloatSecondDuration>(currentTime - previousTime).count();
}

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

	return angle * sfz::g_DEG_TO_RAD_FLOAT;
}

sfz::vec3f tilePosToVector(const s3::Model& model, const s3::Position& tilePos) noexcept
{
	// +0.5f to get the midpoint of the tile
	const float e1f = static_cast<float>(tilePos.e1) + 0.5f;
	const float e2f = static_cast<float>(tilePos.e2) + 0.5f; 
	const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);

	return (e1f * tileWidth - 0.5f) * s3::directionVector(tilePos.side, s3::Coordinate::e1) +
	       (e2f * tileWidth - 0.5f) * s3::directionVector(tilePos.side, s3::Coordinate::e2) +
	       s3::toVector(tilePos.side) * 0.5f;
}

sfz::mat4f tileSpaceRotation(s3::Direction3D side) noexcept
{
	switch (side) {
	case s3::Direction3D::UP: return sfz::identityMatrix<float>();
	case s3::Direction3D::DOWN: return sfz::xRotationMatrix(sfz::g_PI_FLOAT);
	case s3::Direction3D::SOUTH: return sfz::xRotationMatrix(sfz::g_PI_FLOAT/2.0f);
	case s3::Direction3D::NORTH: return sfz::xRotationMatrix(-sfz::g_PI_FLOAT/2.0f);
	case s3::Direction3D::WEST: return sfz::zRotationMatrix(sfz::g_PI_FLOAT/2.0f);
	case s3::Direction3D::EAST: return sfz::zRotationMatrix(-sfz::g_PI_FLOAT/2.0f);
	}
}

} // anonymous namespace

// Game loop functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Called once for each event every frame.
bool handleInput(s3::Model& model, const SDL_Event& event)
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
			isTransparent = globalConfig.mTransparentCube;
			break;
		}
		break;
	}
	return false;
}

// Called once every frame
bool update(s3::Model& model, float delta)
{
	if (isPaused) return false;

	model.update(delta);
	if (model.mGameOver) std::cout << "GAME OVER, Final score: " << model.mScore << std::endl;

	cam.update(model, delta);

	return false;
}

// Called once every frame
void render(sdl::Window& window, const s3::Assets& assets, s3::Model& model, float)
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
	if (isTransparent ||
	    (globalConfig.mTransparentWhenEating && model.mTransparentTimeLeft > 0.f)) {
		glDisable(GL_CULL_FACE);
	} else {
		glEnable(GL_CULL_FACE);
	}

	glViewport(0, 0, window.drawableWidth(), window.drawableHeight());

	glUseProgram(shaderProgram);

	const sfz::mat4f viewProj = projMatrix * cam.mViewMatrix;

	// Only one texture is used when rendering SnakeTiles
	gl::setUniform(shaderProgram, "tex", 0);
	glActiveTexture(GL_TEXTURE0);

	// Render all SnakeTiles
	const float gridWidth = static_cast<float>(model.mCfg.gridWidth);
	const float tileWidth = 1.0f / gridWidth;
	sfz::mat4f borderTransform, spriteTransform;
	s3::SnakeTile* tilePtr;
	s3::Position tilePos;

	const size_t tilesPerSide = model.mCfg.gridWidth*model.mCfg.gridWidth;
	for (size_t side = 0; side < 6; side++) {
		s3::SnakeTile* sidePtr = model.getTilePtr(s3::Position{cam.mSideRenderOrder[side], 0, 0});

		// Hack to correctly print dead snake (part 1)
		if (model.mGameOver && !cam.mRenderTileBorderFirst[side]) {
			s3::Position deadHeadPos = model.getTilePosition(model.mDeadHeadPtr);

			// Tile Sprite Transform
			spriteTransform =
				viewProj *
				sfz::translationMatrix(tilePosToVector(model, deadHeadPos) + s3::toVector(deadHeadPos.side)*0.001f) *
				tileSpaceRotation(deadHeadPos.side) *
				sfz::scalingMatrix(tileWidth) *
				sfz::yRotationMatrix(getTileAngleRad(deadHeadPos.side, tilePtr->from()));

			// Render snake sprite
			gl::setUniform(shaderProgram, "modelViewProj", spriteTransform);
			glBindTexture(GL_TEXTURE_2D, assets.getTileTexture(tilePtr, model.mProgress, model.mGameOver).mHandle);
			if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
			else tile.render();
		}

		if (cam.mRenderTileBorderFirst[side]) {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = model.getTilePosition(tilePtr);

				// Tile Border Transform
				borderTransform =
					viewProj *
					sfz::translationMatrix(tilePosToVector(model, tilePos)) *
					tileSpaceRotation(tilePos.side) *
					sfz::scalingMatrix(tileWidth) *
					sfz::yRotationMatrix(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render tile border
				gl::setUniform(shaderProgram, "modelViewProj", borderTransform);
				glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.mHandle);
				tile.render();

				// Only render tiles with sprites
				if(tilePtr->type() == s3::TileType::EMPTY) continue;

				// Tile Sprite Transform
				spriteTransform =
					viewProj *
					sfz::translationMatrix(tilePosToVector(model, tilePos) + s3::toVector(tilePos.side)*0.001f) *
					tileSpaceRotation(tilePos.side) *
					sfz::scalingMatrix(tileWidth) *
					sfz::yRotationMatrix(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render snake sprite
				gl::setUniform(shaderProgram, "modelViewProj", spriteTransform);
				glBindTexture(GL_TEXTURE_2D, assets.getTileTexture(tilePtr, model.mProgress, model.mGameOver).mHandle);
				if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
				else tile.render();
			}
		} else {
			for (size_t i = 0; i < tilesPerSide; i++) {
				tilePtr = sidePtr + i;
				tilePos = model.getTilePosition(tilePtr);


				// Only render tiles with sprites
				if(tilePtr->type() != s3::TileType::EMPTY) {
					// Tile Sprite Transform
					spriteTransform =
						viewProj *
						sfz::translationMatrix(tilePosToVector(model, tilePos) + s3::toVector(tilePos.side)*0.001f) *
						tileSpaceRotation(tilePos.side) *
						sfz::scalingMatrix(tileWidth) *
						sfz::yRotationMatrix(getTileAngleRad(tilePos.side, tilePtr->from()));

					// Render snake sprite
					gl::setUniform(shaderProgram, "modelViewProj", spriteTransform);
					glBindTexture(GL_TEXTURE_2D, assets.getTileTexture(tilePtr, model.mProgress, model.mGameOver).mHandle);
					if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
					else tile.render();
				}

				// Tile Border Transform
				borderTransform =
					viewProj *
					sfz::translationMatrix(tilePosToVector(model, tilePos)) *
					tileSpaceRotation(tilePos.side) *
					sfz::scalingMatrix(tileWidth) *
					sfz::yRotationMatrix(getTileAngleRad(tilePos.side, tilePtr->from()));

				// Render tile border
				gl::setUniform(shaderProgram, "modelViewProj", borderTransform);
				glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.mHandle);
				tile.render();
			}
		}

		// Hack to correctly print dead snake (part 2)
		if (model.mGameOver && !cam.mRenderTileBorderFirst[side]) {
			
		}
	}

	// Clean up
	glUseProgram(0);
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int main()
{
	// Init global config variable
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	globalConfig.load();
	globalConfig.save(); // Save the sanitized values to avoid user confusion.

	// Init libraries and stuff
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Session sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}};

	if (globalConfig.mMSAA > 0) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, globalConfig.mMSAA);
	}

	sdl::Window window{"snakium³", globalConfig.mWindowResolutionX, globalConfig.mWindowResolutionY,
	     {sdl::WindowFlags::OPENGL, sdl::WindowFlags::RESIZABLE, sdl::WindowFlags::ALLOW_HIGHDPI}};

	gl::Context glContext{window.mPtr, 3, 3, gl::GLContextProfile::CORE};

	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cerr << "GLEW initialization failure:\n" << glewGetErrorString(glewError) << std::endl;
		std::terminate();
	}
	checkGLErrorsMessage("^^^ Above errors caused by glewInit().");

	// Init variables
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	shaderProgram = s3::compileStandardShaderProgram();

	float aspect = static_cast<float>(window.width()) / static_cast<float>(window.height());
	projMatrix = sfz::glPerspectiveProjectionMatrix(cam.mFov, aspect, 0.1f, 50.0f);

	isTransparent = globalConfig.mTransparentCube;

	s3::Assets assets;

	s3::Model model{globalConfig.mModelConfig};

	checkGLErrorsMessage("^^^ Above errors caused by initing variables and loading assets.");

	// Game loop
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool running = true;
	float delta = calculateDelta(); // Call calculateDelta() here to initialize counting.
	SDL_Event event;

	while (running) {
		delta = calculateDelta();

		//std::cout << "Delta = " << delta << ", fps = " << (1.0f / delta) << "\n";

		while (SDL_PollEvent(&event) != 0) if (handleInput(model, event)) running = false;
		if (update(model, delta)) running = false;
		render(window, assets, model, delta);

		SDL_GL_SwapWindow(window.mPtr);

		checkGLErrorsMessage("^^^ Above errors likely caused by game loop.");
	}
	

	return 0;
}

#include "sfz/MSVC12HackOFF.hpp"