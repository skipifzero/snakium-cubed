#include <sfz/SDL.hpp>
#include <GL/glew.h>

/*
// TODO: Not sure if needed, currently compiles fine without on OS X.
#ifdef _WIN32
	#include <windows.h>
#endif
#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	//#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#*/

#include <iostream>
#include <exception> // std::terminate()

void printErrorLog(GLuint thing) noexcept
{
	int logLength;
	glGetShaderiv(thing, GL_INFO_LOG_LENGTH, &logLength);
	char* log = new char[logLength+1];
	glGetShaderInfoLog(thing, logLength, NULL, log);
	std::cout << log << std::endl;
	delete[] log;
}

int main()
{
	sdl::Session sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}};
	
	// OpenGL 3.3 Core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	sdl::Window window{"snakium³", 400, 400,
	     {sdl::WindowFlags::OPENGL, sdl::WindowFlags::RESIZABLE, sdl::WindowFlags::ALLOW_HIGHDPI}};

	SDL_GLContext context = SDL_GL_CreateContext(window.mPtr);

	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cout << "GLEW initialization failure:\n" << glewGetErrorString(glewError) << std::endl;
		std::terminate();
	}


	const GLchar* vertexShaderSrc = R"(
		#version 140

		in vec3 position;
		in vec3 color;
		out vec3 outColor;

		void main()
		{
			gl_Position = vec4(position, 1);
			outColor = color;
		}
	)";
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	{
		int compileSuccess;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileSuccess);
		if (!compileSuccess) {
			printErrorLog(vertexShader);
			return 1;
		}
	}

	const GLchar* fragmentShaderSrc = R"(
		#version 140

		precision highp float;

		in vec3 outColor;
		out vec4 fragmentColor;

		void main()
		{
			fragmentColor = vec4(outColor, 1);	
		}
	)";
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	{
		int compileSuccess;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileSuccess);
		if (!compileSuccess) {
			printErrorLog(fragmentShader);
			return 1;
		}
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glBindAttribLocation(shaderProgram, 0, "position");
	glBindAttribLocation(shaderProgram, 1, "color");
	glBindFragDataLocation(shaderProgram, 0, "fragmentColor");

	glLinkProgram(shaderProgram);
	{
		GLint linkSuccess = 0;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
		if (!linkSuccess) {
			printErrorLog(shaderProgram);
			return 1;
		}
	}


	const float positions[] = {
		//x, y, z
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};
	GLuint posBuffer;
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	const float colors[] = {
		//R, G, B
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	GLuint colBuffer;
	glGenBuffers(1, &colBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);


	float lookAtMe = 0.0f;
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
					lookAtMe += 0.1f;
					break;
				case SDLK_DOWN:
					lookAtMe -= 0.1f;
					break;
				}
				break;
			default:
				std::cout << "Unhandled event: " << std::to_string(event.type) << "\n";
			}
		}

		// Rendering
		glClearColor(lookAtMe, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, window.width(), window.height());

		glDisable(GL_CULL_FACE);

		glUseProgram(shaderProgram);

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(0);

		SDL_GL_SwapWindow(window.mPtr);
	}

	SDL_GL_DeleteContext(context);
	return 0;
}