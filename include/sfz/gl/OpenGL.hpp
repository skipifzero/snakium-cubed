#pragma once
#ifndef SFZ_GL_OPEN_GL_HPP
#define SFZ_GL_OPEN_GL_HPP

// GLEW
#include <GL/glew.h>

// OpenGL headers
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef near
#undef far

#elif __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//#include <GLUT/glut.h>

#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#endif