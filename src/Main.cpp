#include <iostream>
#include <exception> // std::terminate()
#include <string>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "SnakiumCubedShaders.hpp"
#include "TileObject.hpp"
#include "MainLoop.hpp"

// Variables
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
GLuint shaderProgram = -1;

sfz::vec3f camPos{0, 0, 3};
sfz::vec3f camTarget{0, 0, 0};

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


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

// Game loop functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool update(float delta) noexcept
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			return true;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return true;
			case SDLK_UP:
				camPos = transformPoint(sfz::xRotationMatrix(-0.1f*sfz::g_PI_FLOAT), camPos);
				//viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
				break;
			case SDLK_DOWN:
				camPos = transformPoint(sfz::xRotationMatrix(0.1f*sfz::g_PI_FLOAT), camPos);
				//viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
				break;
			case SDLK_RIGHT:
				camPos = transformPoint(sfz::yRotationMatrix(0.1f*sfz::g_PI_FLOAT), camPos);
				//viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
				break;
			case SDLK_LEFT:
				camPos = transformPoint(sfz::yRotationMatrix(-0.1f*sfz::g_PI_FLOAT), camPos);
				//viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
				break;
			}
		//default:
			//std::cout << "Unhandled event: " << std::to_string(event.type) << "\n";
		}
	}
	return false;
}

bool render(sdl::Window& window, float delta) noexcept
{
	glActiveTexture(GL_TEXTURE0);

	gl::Texture cubeSideTex{"assets/128pix/head_d2u_f2_128.png"};
	gl::Texture floorTex{"assets/128pix/button_middle_touched_128.png"};

	checkGLErrorsMessage("^^^ Above errors caused by texture loading.");

	sfz::mat4f viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
	sfz::mat4f projMatrix = sfz::glPerspectiveProjectionMatrix(60.0f,
							        window.width()/window.height(), 0.1f, 50.0f);

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

	SDL_GL_SwapWindow(window.mPtr);

	checkGLErrorsMessage("^^^ Above errors likely caused by rendering loop.");
	return false;
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int main()
{
	s3::MainLoop gameLoop{update, render};

	shaderProgram = s3::compileStandardShaderProgram();

	gameLoop.run();

	return 0;
}