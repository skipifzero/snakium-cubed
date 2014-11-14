#include "sfz/sdl/GLTexture.hpp"

namespace sdl {
	
namespace {

GLuint loadTexture(const std::string& path) noexcept
{
	// Load specified surface.
	SDL_Surface* surface = NULL;
	surface = IMG_Load(path.c_str());
	if (surface == NULL) {
		std::cout << "Unable to load image at: " << path << ", error: " << IMG_GetError();
		std::terminate();
	}
	
	// Convert loaded surface to correct image format.
	SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
	free(surface);
	if (formattedSurface == NULL) {
		std::cout << "Unable to convert surface format: " << SDL_GetError() << std::endl;
		std::terminate();
	}
	
	// TODO: Figure out if this is necessary.
	glEnable(GL_TEXTURE_2D);
	
	// Creating OpenGL Texture from formatted surface.
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, formattedSurface->w, formattedSurface->h, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);
	SDL_FreeSurface(formattedSurface);
	sfz::checkAllGLErrors();
	
	// TODO: Mipmaps and anistropic filtering.
	
	return texture;
}
	
} // namespace

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GLTexture::GLTexture(const std::string& path) noexcept
:
	mHandle(loadTexture(path))
{
	// Initialization complete.
}
	
GLTexture::~GLTexture() noexcept
{
	glDeleteTextures(1, &mHandle);
}
	
} // namespace sdl
