#include <iostream>
#include <exception> // std::terminate()
#include <string>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "SnakiumCubedShaders.hpp"

void checkGLErrorsMessage(const std::string& msg) noexcept
{
	if (gl::checkAllGLErrors()) {
		std::cerr << msg << std::endl;
	}
}

void setUniform(int location, const sfz::mat4f& matrix) noexcept
{
	glUniformMatrix4fv(location, 1, false, matrix.glPtr());
}

void setUniform(GLuint shaderProgram, const std::string& name, const sfz::mat4f& matrix) noexcept
{
	int loc = glGetUniformLocation(shaderProgram, name.c_str());
	setUniform(loc, matrix);
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


	// Rectangle
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	const float positions[] = {
		// x,    y,    z
		-1.0f, -1.0f, -2.0f,
		-1.0f, 1.0f, -10.0f,
		1.0f, -1.0f, -2.0f,
		1.0f, 1.0f, -10.0f
	};

	const int indices[] = {
		0, 1, 2, // t1
		1, 2, 3 // t2
	};

	const float texcoords[] = {
		// u,    v
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	// Buffer objects
	GLuint posBuffer;
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	GLuint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint texCoordBuffer;
	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);


	// Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	checkGLErrorsMessage("^^^ Above errors caused by Rectangle");


	// Compile shaders and set up modelViewProj matrix
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GLuint shaderProgram = s3::compileStandardShaderProgram();
	sfz::vec3f camPos{0, 0, 0};
	sfz::mat4f viewMatrix = sfz::lookAt(camPos, sfz::vec3f{0, 0, -5}, sfz::vec3f{0,1,0});
	sfz::mat4f projMatrix = sfz::glPerspectiveProjectionMatrix(45.0f,
							        window.width()/window.height(), 0.1f, 50.0f);
	sfz::mat4f modelViewProjMatrix = projMatrix * viewMatrix;
	glUseProgram(shaderProgram);
	setUniform(shaderProgram, "modelViewProj", modelViewProjMatrix);

	checkGLErrorsMessage("^^^ Above errors caused by compile shader.");

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

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(shaderProgram);

		sfz::mat4f modelToViewProj = sfz::scalingMatrix(1.0f, 1.0f, 0.0f); // Orthogonal projection matrix.
		int mLoc = glGetUniformLocation(shaderProgram, "modelToViewProj");
		glUniformMatrix4fv(mLoc, 1, false, modelToViewProj.glPtr());

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		int tLoc = glGetUniformLocation(shaderProgram, "tex");
		glUniform1i(tLoc, 0);
		//glBindTexture(GL_TEXTURE_2D, texture.mHandle);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glUseProgram(0);

		SDL_GL_SwapWindow(window.mPtr);

		checkGLErrorsMessage("^^^ Above errors likely caused by rendering loop.");
	}

	return 0;
}