#include <iostream>
#include <exception> // std::terminate()
#include <string>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "SnakiumCubedShaders.hpp"
#include "TileObject.hpp"

// Variables
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
GLuint shaderProgram = -1;

sfz::vec3f camPos{0, 0, 3};
sfz::vec3f camTarget{0, 0, 0};
sfz::vec3f camUp{0, 1, 0};
sfz::mat4f viewMatrix = sfz::lookAt(camPos, camTarget, camUp);
sfz::mat4f projMatrix;

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
bool render(sdl::Window& window, float)
{
	glActiveTexture(GL_TEXTURE0);

	gl::Texture cubeSideTex{"assets/128pix/head_d2u_f2_128.png"};
	gl::Texture floorTex{"assets/128pix/button_middle_touched_128.png"};

	checkGLErrorsMessage("^^^ Above errors caused by texture loading.");

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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTex.mHandle);
	gl::setUniform(shaderProgram, "tex", 0);

	// Ground
	gl::setUniform(shaderProgram, "modelViewProj",
		viewProj
		* sfz::translationMatrix(0.0f, -4.0f, 0.0f)
		* sfz::scalingMatrix(10.0f));
	tile.render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeSideTex.mHandle);
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


	glUseProgram(0);

	checkGLErrorsMessage("^^^ Above errors likely caused by rendering loop.");
	return false;
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int main()
{
	// Init libraries and stuff
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Session sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}};

	sdl::Window window{"snakium³", 400, 400,
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
		if (render(window, delta)) running = false;

		SDL_GL_SwapWindow(window.mPtr);
	}
	

	return 0;
}