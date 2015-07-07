#pragma once
#ifndef SFZ_GL_OPEN_GL_HPP
#define SFZ_GL_OPEN_GL_HPP

// SDL, needed for loading textures.
#include "sfz/SDL.hpp"

// GLEW
#include <GL/glew.h>

// OpenGL headers
#ifdef _WIN32
	#define NOMINMAX
	#include <windows.h>
	#undef near
	#undef far
#endif
#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	//#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#endif