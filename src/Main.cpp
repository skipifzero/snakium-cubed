#include <iostream>
#include <exception> // std::terminate()
#include <string>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "SnakiumCubedShaders.hpp"
#include "TileObject.hpp"
#include "Assets.hpp"
#include "GameLogic.hpp"

// Variables
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
GLuint shaderProgram = -1;

sfz::vec3f camPos{0, 0, 2};
sfz::vec3f camTarget{0, 0, 0};
sfz::vec3f camUp{0, 1, 0};
sfz::mat4f viewMatrix = sfz::lookAt(camPos, camTarget, camUp);
sfz::mat4f projMatrix;

s3::S3Model model{4};

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

sfz::vec3f transformPoint(const sfz::mat4f& transformation, const sfz::vec3f& point) noexcept
{
	sfz::vec4f point4{point[0], point[1], point[2], 1.0f};
	point4 = transformation * point4;
	return sfz::vec3f{point4[0], point4[1], point4[2]};
}

float getTileAngleRad(s3::TileDirection from) noexcept
{
	switch (from) {
	case s3::TileDirection::UP: return (270.0f - 90.0f) * sfz::g_DEG_TO_RAD_FLOAT;
	case s3::TileDirection::DOWN: return (90.0f - 90.0f) * sfz::g_DEG_TO_RAD_FLOAT;
	case s3::TileDirection::LEFT: return (0.0f - 90.0f) * sfz::g_DEG_TO_RAD_FLOAT;
	case s3::TileDirection::RIGHT: return (180.0f - 90.0f) * sfz::g_DEG_TO_RAD_FLOAT;
	}
}

bool isNoTurn(s3::TileDirection from, s3::TileDirection to) noexcept
{
	switch (from) {
	case s3::TileDirection::UP:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return true;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return false;
		}
	case s3::TileDirection::DOWN:
		switch (to) {
		case s3::TileDirection::UP: return true;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return false;
		}
	case s3::TileDirection::LEFT:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return true;
		}
	case s3::TileDirection::RIGHT:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return true;
		case s3::TileDirection::RIGHT: return false;
		}
	}
}

bool isLeftTurn(s3::TileDirection from, s3::TileDirection to) noexcept
{
	switch (from) {
	case s3::TileDirection::UP:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return true;
		}
	case s3::TileDirection::DOWN:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return true;
		case s3::TileDirection::RIGHT: return false;
		}
	case s3::TileDirection::LEFT:
		switch (to) {
		case s3::TileDirection::UP: return true;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return false;
		}
	case s3::TileDirection::RIGHT:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return true;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return false;
		}
	}
}

bool isRightTurn(s3::TileDirection from, s3::TileDirection to) noexcept
{
	switch (from) {
	case s3::TileDirection::UP:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return true;
		case s3::TileDirection::RIGHT: return false;
		}
	case s3::TileDirection::DOWN:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return true;
		}
	case s3::TileDirection::LEFT:
		switch (to) {
		case s3::TileDirection::UP: return false;
		case s3::TileDirection::DOWN: return true;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return false;
		}
	case s3::TileDirection::RIGHT:
		switch (to) {
		case s3::TileDirection::UP: return true;
		case s3::TileDirection::DOWN: return false;
		case s3::TileDirection::LEFT: return false;
		case s3::TileDirection::RIGHT: return false;
		}
	}
}


GLuint getTileTexture(const s3::Assets& assets, s3::TileType tileType,
                      s3::TileDirection from, s3::TileDirection to, float progress) noexcept
{
	bool isTurn = !isNoTurn(from, to);

	switch (tileType) {
	case s3::TileType::EMPTY: return assets.TILE_BORDER.mHandle;
	case s3::TileType::OBJECT: return assets.OBJECT.mHandle;
	case s3::TileType::BONUS_OBJECT: return assets.BONUS_OBJECT.mHandle;

	case s3::TileType::HEAD:
		if (progress <= 0.5f) { // Frame 1
			return assets.HEAD_D2U_F1.mHandle;
		} else { // Frame 2
			return assets.HEAD_D2U_F2.mHandle;
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
		std::cerr << "HEAD_DIGESTING should never happen.\n";
		std::terminate();
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

} // anonymous namespace

// Game loop functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Called once for each event every frame.
bool handleInput(const SDL_Event& event)
{
	switch (event.type) {
	case SDL_QUIT:
		return true;
	case SDL_WINDOWEVENT:
		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			float w = static_cast<float>(event.window.data1);
			float h = static_cast<float>(event.window.data2);
			projMatrix = sfz::glPerspectiveProjectionMatrix(60.0f, w/h, 0.1f, 50.0f);
		}
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			return true;
		case SDLK_UP:
			camPos = transformPoint(sfz::xRotationMatrix(-0.1f*sfz::g_PI_FLOAT), camPos);
			viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
			break;
		case SDLK_DOWN:
			camPos = transformPoint(sfz::xRotationMatrix(0.1f*sfz::g_PI_FLOAT), camPos);
			viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
			break;
		case SDLK_RIGHT:
			camPos = transformPoint(sfz::yRotationMatrix(0.1f*sfz::g_PI_FLOAT), camPos);
			viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
			break;
		case SDLK_LEFT:
			camPos = transformPoint(sfz::yRotationMatrix(-0.1f*sfz::g_PI_FLOAT), camPos);
			viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
			break;
		}
	//default:
		//std::cout << "Unhandled event: " << std::to_string(event.type) << "\n";
	}
	return false;
}

// Called once every frame
bool update(float)
{

	return false;
}

// Called once every frame
void render(sdl::Window& window, const s3::Assets& assets, float)
{
	glActiveTexture(GL_TEXTURE0);

	s3::TileObject tile{false, false};
	s3::TileObject xFlippedTile{true, false};

	// Clearing screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, window.width()*2, window.height()*2);

	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);

	sfz::mat4f viewProj = projMatrix * viewMatrix;


	gl::setUniform(shaderProgram, "tex", 0);
	glActiveTexture(GL_TEXTURE0);

	std::uint8_t* current = model.mBytes;
	std::uint8_t* max = model.mBytes + model.mByteCount;
	s3::TileType tileType;
	s3::TileDirection from, to;
	float x, y, z;
	s3::CubeSide cubeSide;
	const float gridWidth = static_cast<float>(model.mGridWidth);
	const float halfGridWidth = gridWidth * 0.5f;
	const float tileWidth = 1.0f / gridWidth;
	const float halfTileWidth = tileWidth * 0.5f;
	sfz::mat4f transform;
	while (current < max) {
		tileType = s3::tileSideType(current[0]);
		from = s3::tileSideFrom(current[0]);
		to = s3::tileSideTo(current[0]);
		x = static_cast<float>(s3::getFourMSBs(current[1]));
		y = static_cast<float>(s3::getFourLSBs(current[1]));
		z = static_cast<float>(s3::getFourMSBs(current[2]));
		cubeSide = static_cast<s3::CubeSide>(s3::getFourLSBs(current[2]));

		transform = viewProj;

		// Translation
		transform = transform * sfz::translationMatrix(x*tileWidth - 0.5f,
		                                               y*tileWidth - 0.5f,
		                                               z*tileWidth - 0.5f);

		// Rotation
		switch (cubeSide) {
		case s3::CubeSide::TOP:
			transform = transform
					  * sfz::translationMatrix(halfTileWidth, tileWidth, halfTileWidth);
			break;
		case s3::CubeSide::BOTTOM:
			transform = transform
					  * sfz::translationMatrix(halfTileWidth, 0.0f, halfTileWidth)
					  * sfz::xRotationMatrix(sfz::g_PI_FLOAT);
			break;
		case s3::CubeSide::FRONT:
			transform = transform
					  * sfz::translationMatrix(halfTileWidth, halfTileWidth, tileWidth)
					  * sfz::xRotationMatrix(sfz::g_PI_FLOAT/2.0f);
			break;
		case s3::CubeSide::BACK:
			transform = transform
					  * sfz::translationMatrix(halfTileWidth, halfTileWidth, 0.0f)
					  * sfz::xRotationMatrix(-sfz::g_PI_FLOAT/2.0f);
			break;
		case s3::CubeSide::LEFT:
			transform = transform
					  * sfz::translationMatrix(0.0f, halfTileWidth, halfTileWidth)
					  * sfz::zRotationMatrix(sfz::g_PI_FLOAT/2.0f);
			break;
		case s3::CubeSide::RIGHT:
			transform = transform
					  * sfz::translationMatrix(tileWidth, halfTileWidth, halfTileWidth)
					  * sfz::zRotationMatrix(-sfz::g_PI_FLOAT/2.0f);
			break;
		}

		// Scaling
		transform = transform * sfz::scalingMatrix(tileWidth);

		gl::setUniform(shaderProgram, "modelViewProj", transform);

		glBindTexture(GL_TEXTURE_2D, assets.TILE_BORDER.mHandle);
		tile.render();

		glBindTexture(GL_TEXTURE_2D, getTileTexture(assets, tileType, from, to, 0.5f));
		if (isLeftTurn(from, to)) {
			xFlippedTile.render();
		} else {
			tile.render();
		}

		current += 3;
	}

	glUseProgram(0);
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int main()
{
	// Init libraries and stuff
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Session sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}};

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

		std::cout << "Delta = " << delta << ", fps = " << (1.0f / delta) << "\n";

		while (SDL_PollEvent(&event) != 0) if (handleInput(event)) running = false;
		if (update(delta)) running = false;
		render(window, assets, delta);

		SDL_GL_SwapWindow(window.mPtr);

		checkGLErrorsMessage("^^^ Above errors likely caused by game loop.");
	}
	

	return 0;
}