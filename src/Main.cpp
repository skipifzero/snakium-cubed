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

sfz::vec3f camPos{0, 0, 3};
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

	s3::TileObject tile;

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
	std::uint8_t x, y, z;
	s3::CubeSide cubeSide;
	const size_t gridWidth = model.mGridWidth;
	sfz::mat4f transform;
	while (current < max) {
		tileType = s3::tileSideType(current[0]);
		from = s3::tileSideFrom(current[0]);
		to = s3::tileSideTo(current[0]);
		x = s3::getFourMSBs(current[1]);
		y = s3::getFourLSBs(current[1]);
		z = s3::getFourMSBs(current[2]);
		cubeSide = static_cast<s3::CubeSide>(s3::getFourLSBs(current[2]));

		transform = viewProj;

		// Translation
		// TODO: Implement

		// Rotation
		switch (cubeSide) {
			case s3::CubeSide::TOP:
				// Do nothing.
				break;
			case s3::CubeSide::BOTTOM:
				transform = transform * sfz::xRotationMatrix(sfz::g_PI_FLOAT);
				break;
			case s3::CubeSide::FRONT:
				transform = transform * sfz::xRotationMatrix(sfz::g_PI_FLOAT/2.0f);
				break;
			case s3::CubeSide::BACK:
				transform = transform * sfz::xRotationMatrix(-sfz::g_PI_FLOAT/2.0f);
				break;
			case s3::CubeSide::LEFT:
				transform = transform * sfz::zRotationMatrix(sfz::g_PI_FLOAT/2.0f);
				break;
			case s3::CubeSide::RIGHT:
				transform = transform * sfz::zRotationMatrix(-sfz::g_PI_FLOAT/2.0f);
				break;
		}

		// Resizing
		// TODO: Implement

		gl::setUniform(shaderProgram, "modelViewProj", transform);

		glBindTexture(GL_TEXTURE_2D, assets.TILE_BORDER.mHandle);
		tile.render();

		glBindTexture(GL_TEXTURE_2D, assets.HEAD_D2U_F2.mHandle);
		tile.render();

		current += 3;
	}

	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, assets.FILLED.mHandle);
	gl::setUniform(shaderProgram, "tex", 0);

	// Ground
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, -4.0f, 0.0f)
		* sfz::scalingMatrix(10.0f));
	tile.render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, assets.HEAD_D2U_F2.mHandle);
	gl::setUniform(shaderProgram, "tex", 0);

	// Bottom
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, -0.5f, 0.0f)
		* sfz::xRotationMatrix(sfz::g_PI_FLOAT));
	tile.render();

	// Top
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, 0.5f, 0.0f));
	tile.render();

	// Front
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, 0.0f, 0.5f)
		* sfz::xRotationMatrix(sfz::g_PI_FLOAT/2.0f));
	tile.render();

	// Back
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, 0.0f, -0.5f)
		* sfz::xRotationMatrix(-sfz::g_PI_FLOAT/2.0f));
	tile.render();

	// Left
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(-0.5f, 0.0f, 0.0f)
		* sfz::zRotationMatrix(sfz::g_PI_FLOAT/2.0f));
	tile.render();

	// Right
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.5f, 0.0f, 0.0f)
		* sfz::zRotationMatrix(-sfz::g_PI_FLOAT/2.0f));
	tile.render();


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, assets.TILE_BORDER.mHandle);
	gl::setUniform(shaderProgram, "tex", 0);

	// Bottom
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, -0.5f, 0.0f)
		* sfz::xRotationMatrix(sfz::g_PI_FLOAT));
	tile.render();

	// Top
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, 0.5f, 0.0f));
	tile.render();

	// Front
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, 0.0f, 0.5f)
		* sfz::xRotationMatrix(sfz::g_PI_FLOAT/2.0f));
	tile.render();

	// Back
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, 0.0f, -0.5f)
		* sfz::xRotationMatrix(-sfz::g_PI_FLOAT/2.0f));
	tile.render();

	// Left
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(-0.5f, 0.0f, 0.0f)
		* sfz::zRotationMatrix(sfz::g_PI_FLOAT/2.0f));
	tile.render();

	// Right
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.5f, 0.0f, 0.0f)
		* sfz::zRotationMatrix(-sfz::g_PI_FLOAT/2.0f));
	tile.render();
	*/

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