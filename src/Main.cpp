#include <iostream>
#include <exception> // std::terminate()
#include <string>
#include <chrono>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "Assets.hpp"
#include "Camera.hpp"
#include "GameLogic.hpp"
#include "SnakiumCubedShaders.hpp"
#include "TileObject.hpp"

#undef main // Remove SDL hack until we can get it to compile properly

// Variables
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
s3::Config getConfig(void) noexcept
{
	s3::Config cfg;
	cfg.gridWidth = 3;
	cfg.tilesPerSecond = 2.0f;
	return cfg;
}

s3::Model model{getConfig()};

GLuint shaderProgram;

s3::Direction3D upDir = s3::Direction3D::UP;
s3::Direction3D lastCubeSide = model.getTilePosition(model.mHeadPtr).side;
s3::Camera cam;
sfz::mat4f projMatrix;

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
	if (gl::checkAllGLErrors()) {
		std::cerr << msg << std::endl;
	}
}

/*sfz::vec3f transformPoint(const sfz::mat4f& transformation, const sfz::vec3f& point) noexcept
{
	sfz::vec4f point4{point[0], point[1], point[2], 1.0f};
	point4 = transformation * point4;
	return sfz::vec3f{point4[0], point4[1], point4[2]};
}*/

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

GLuint getTileTexture(const s3::Assets& assets, s3::SnakeTile* tilePtr, float progress) noexcept
{
	bool isTurn = s3::isTurn(tilePtr->from(), tilePtr->to());

	switch (tilePtr->type()) {
	case s3::TileType::EMPTY: return assets.TILE_FACE.mHandle;
	case s3::TileType::OBJECT: return assets.OBJECT.mHandle;
	case s3::TileType::BONUS_OBJECT: return assets.BONUS_OBJECT.mHandle;

	case s3::TileType::HEAD:
		if (progress <= 0.5f) { // Frame 1
			return assets.HEAD_D2U_F1.mHandle;
		} else { // Frame 2
			return assets.HEAD_D2U_F2.mHandle;
		}
	case s3::TileType::PRE_HEAD:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return assets.PRE_HEAD_D2U_F1.mHandle;
			else return assets.PRE_HEAD_D2R_F1.mHandle;
		} else { // Frame 2
			if (!isTurn) return assets.BODY_D2U.mHandle;
			else return assets.BODY_D2R.mHandle;
		}
	case s3::TileType::BODY:
		if (!isTurn) return assets.BODY_D2U.mHandle;
		else return assets.BODY_D2R.mHandle;
	case s3::TileType::TAIL:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return assets.TAIL_D2U_F1.mHandle;
			else return assets.TAIL_D2R_F1.mHandle;
		} else { // Frame 2
			if (!isTurn) return assets.TAIL_D2U_F2.mHandle;
			else return assets.TAIL_D2R_F2.mHandle;
		}

	case s3::TileType::HEAD_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			return assets.HEAD_D2U_F1.mHandle;
		} else { // Frame 2
			return assets.HEAD_D2U_F2.mHandle;
		}
	case s3::TileType::PRE_HEAD_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return assets.PRE_HEAD_D2U_DIG_F1.mHandle;
			else return assets.PRE_HEAD_D2R_DIG_F1.mHandle;
		} else { // Frame 2
			if (!isTurn) return assets.BODY_D2U_DIG.mHandle;
			else return assets.BODY_D2R_DIG.mHandle;
		}
	case s3::TileType::BODY_DIGESTING:
		if (!isTurn) return assets.BODY_D2U_DIG.mHandle;
		else return assets.BODY_D2R_DIG.mHandle;
	case s3::TileType::TAIL_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return assets.TAIL_D2U_DIG_F1.mHandle;
			else return assets.TAIL_D2R_DIG_F1.mHandle;
		} else { // Frame 2
			if (!isTurn) return assets.TAIL_D2U_DIG_F2.mHandle;
			else return assets.TAIL_D2R_DIG_F2.mHandle;
		}
	}
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
bool handleInput(const SDL_Event& event)
{
	switch (event.type) {
	case SDL_QUIT: return true;
	case SDL_WINDOWEVENT:
		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			float w = static_cast<float>(event.window.data1);
			float h = static_cast<float>(event.window.data2);
			projMatrix = sfz::glPerspectiveProjectionMatrix(60.0f, w/h, 0.1f, 50.0f);
		}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE: return true;
		case SDLK_SPACE:
			isPaused = !isPaused;
			break;
		case SDLK_UP:
			model.changeDirection(upDir, s3::Direction2D::UP);
			break;
		case SDLK_DOWN:
			model.changeDirection(upDir, s3::Direction2D::DOWN);
			break;
		case SDLK_LEFT:
			model.changeDirection(upDir, s3::Direction2D::LEFT);
			break;
		case SDLK_RIGHT:
			model.changeDirection(upDir, s3::Direction2D::RIGHT);
			break;
		}
	}
	return false;
}

// Called once every frame
bool update(float delta)
{
	if (isPaused) return false;

	model.update(delta);
	if (model.mGameOver) std::cout << "GAME OVER, Final score: " << model.mScore << std::endl;

	auto headPos = model.getTilePosition(model.mHeadPtr);
	auto preHeadPos = model.adjacent(headPos, model.mHeadPtr->from());

	if (lastCubeSide != headPos.side) {
		if (headPos.side == upDir) upDir = s3::opposite(lastCubeSide);
		else if (headPos.side == opposite(upDir)) upDir = lastCubeSide;
		lastCubeSide = headPos.side;
		std::cout << headPos.side << std::endl;
	}

	static const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);

	s3::Direction3D preHeadTo = mapDefaultUp(preHeadPos.side, model.getTilePtr(preHeadPos)->to());
	sfz::vec3f currentDir = s3::toVector(preHeadTo);
	//const sfz::vec3f currentDir = toVector(mapDefaultUp(headPos.side, model.mHeadPtr->to()));
	sfz::vec3f tileVecPos = tilePosToVector(model, preHeadPos) + currentDir*model.mProgress*tileWidth;

	cam.update(upDir, tileVecPos);

	return false;
}

// Called once every frame
void render(sdl::Window& window, const s3::Assets& assets, float)
{
	static s3::TileObject tile{false, false};
	static s3::TileObject xFlippedTile{true, false};

	//glClearDepth(1.0f);
	//glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	

	// Clearing screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable culling
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, window.drawableWidth(), window.drawableHeight());

	glUseProgram(shaderProgram);

	const sfz::mat4f viewProj = projMatrix * cam.mViewMatrix;

	// Only one texture is used when rendering SnakeTiles
	gl::setUniform(shaderProgram, "tex", 0);
	glActiveTexture(GL_TEXTURE0);

	// Render all SnakeTiles
	const float gridWidth = static_cast<float>(model.mCfg.gridWidth);
	const float tileWidth = 1.0f / gridWidth;
	sfz::mat4f transform;
	s3::SnakeTile* tilePtr;
	s3::Position tilePos;

	// Render the empty tiles
	for (std::size_t i = 0; i < model.mTileCount; i++) {
		tilePtr = model.mTiles + i;
		tilePos = model.getTilePosition(tilePtr);

		// Transform
		transform =
		    viewProj *
			sfz::translationMatrix(tilePosToVector(model, tilePos)) *
			tileSpaceRotation(tilePos.side) *
			sfz::scalingMatrix(tileWidth) *
			sfz::yRotationMatrix(getTileAngleRad(tilePos.side, tilePtr->from()));

		gl::setUniform(shaderProgram, "modelViewProj", transform);

		// Render tile border
		glBindTexture(GL_TEXTURE_2D, assets.TILE_FACE.mHandle);
		tile.render();
	}

	// Render the snake tiles
	for (std::size_t i = 0; i < model.mTileCount; i++) {
		tilePtr = model.mTiles + i;
		tilePos = model.getTilePosition(tilePtr);

		if(tilePtr->type() == s3::TileType::EMPTY) continue;

		// Transform
		transform =
			viewProj *
			sfz::translationMatrix(tilePosToVector(model, tilePos) + s3::toVector(tilePos.side)*0.001f) *
			tileSpaceRotation(tilePos.side) *
			sfz::scalingMatrix(tileWidth) *
			sfz::yRotationMatrix(getTileAngleRad(tilePos.side, tilePtr->from()));

		gl::setUniform(shaderProgram, "modelViewProj", transform);

		// Render snake sprite
		glBindTexture(GL_TEXTURE_2D, getTileTexture(assets, tilePtr, model.mProgress));
		if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
		else tile.render();

		// Transform to inside
		transform =
			viewProj *
			sfz::translationMatrix(tilePosToVector(model, tilePos) + s3::toVector(s3::opposite(tilePos.side))*0.001f) *
			tileSpaceRotation(s3::opposite(tilePos.side)) *
			sfz::scalingMatrix(tileWidth) *
			sfz::yRotationMatrix(getTileAngleRad(tilePos.side, tilePtr->from()));

		gl::setUniform(shaderProgram, "modelViewProj", transform);

		// Render inside snake sprite
		glBindTexture(GL_TEXTURE_2D, getTileTexture(assets, tilePtr, model.mProgress));
		if (isLeftTurn(tilePtr->from(), tilePtr->to())) xFlippedTile.render();
		else tile.render();
	}

	// Clean up
	glUseProgram(0);
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int main(int argc, char* argv[])
{
	// Init libraries and stuff
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Session sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}};

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	sdl::Window window{"snakium³", 500, 500,
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

	projMatrix = sfz::glPerspectiveProjectionMatrix(60.0f, window.width()/window.height(),
	                                                0.1f, 50.0f);

	s3::Assets assets;

	checkGLErrorsMessage("^^^ Above errors caused by initing variables and loading assets.");

	// Game loop
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool running = true;
	float delta = calculateDelta(); // Call calculateDelta() here to initialize counting.
	SDL_Event event;

	while (running) {
		delta = calculateDelta();

		//std::cout << "Delta = " << delta << ", fps = " << (1.0f / delta) << "\n";

		while (SDL_PollEvent(&event) != 0) if (handleInput(event)) running = false;
		if (update(delta)) running = false;
		render(window, assets, delta);

		SDL_GL_SwapWindow(window.mPtr);

		checkGLErrorsMessage("^^^ Above errors likely caused by game loop.");
	}
	

	return 0;
}