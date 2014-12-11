#include <iostream>
#include <exception> // std::terminate()
#include <string>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "SnakiumCubedShaders.hpp"
#include "TileObject.hpp"

void checkGLErrorsMessage(const std::string& msg) noexcept
{
	if (gl::checkAllGLErrors()) {
		std::cerr << msg << std::endl;
	}
}

int main()
{
	// Setup
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Session sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}};

	sdl::Window window{"snakium³", 400, 400,
	     {sdl::WindowFlags::OPENGL, sdl::WindowFlags::RESIZABLE, sdl::WindowFlags::ALLOW_HIGHDPI}};

	// OpenGL 3.3 Core
	gl::Context glContext{window.mPtr, 3, 3, gl::GLContextProfile::CORE};

	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cerr << "GLEW initialization failure:\n" << glewGetErrorString(glewError) << std::endl;
		std::terminate();
	}
	checkGLErrorsMessage("^^^ Above errors caused by glewInit().");

	
	// Other
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	s3::TileObject tile;

	// Compile shaders
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GLuint shaderProgram = s3::compileStandardShaderProgram();


	// Camera variables
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sfz::vec3f camPos{0, -4, 0};
	sfz::vec3f camTarget{0, 0.01f, 0};
	sfz::mat4f viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
	sfz::mat4f projMatrix = sfz::glPerspectiveProjectionMatrix(60.0f,
							        window.width()/window.height(), 0.1f, 50.0f);


	// Texture
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	glActiveTexture(GL_TEXTURE0);

	gl::Texture texture{"assets/128pix/head_d2u_f2_128.png"};
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.mHandle);

	glUseProgram(shaderProgram);
	int texLoc = glGetUniformLocation(shaderProgram, "tex");
	glUniform1i(texLoc, 0);

	checkGLErrorsMessage("^^^ Above errors caused by texture loading.");

	// Event & rendering loop
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool running = true;
	SDL_Event event;
	while (running) {

		// Event handling
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_UP:
					camPos[2] -= 0.1f;
					viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
					break;
				case SDLK_DOWN:
					camPos[2] += 0.1f;
					viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
					break;
				case SDLK_LEFT:
					camPos[0] -= 0.1f;
					viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
					break;
				case SDLK_RIGHT:
					camPos[0] += 0.1f;
					viewMatrix = sfz::lookAt(camPos, camTarget, sfz::vec3f{0,1,0});
					break;
				}
			//default:
				//std::cout << "Unhandled event: " << std::to_string(event.type) << "\n";
			}
		}

		// Rendering

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

		gl::setUniform(shaderProgram, "modelViewProj", projMatrix * viewMatrix);
		gl::setUniform(shaderProgram, "tex", 0);


		tile.render();


		glUseProgram(0);

		SDL_GL_SwapWindow(window.mPtr);

		checkGLErrorsMessage("^^^ Above errors likely caused by rendering loop.");
	}

	return 0;
}