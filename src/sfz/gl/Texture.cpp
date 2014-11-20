#include "sfz/gl/Texture.hpp"

namespace gl {
	
namespace {

// Simple function that flips a surface by byte-level manipulation.
void flipSurface(SDL_Surface* surface) noexcept
{
	// Constants & pointers
	const size_t pixelByteCount = surface->format->BytesPerPixel;
	const size_t w = surface->w;
	const size_t h = surface->h;
	const size_t imageByteCount = pixelByteCount * w * h;
	Uint8* surfacePixels = static_cast<Uint8*>(surface->pixels);
	Uint8* pixelBuffer = new Uint8[imageByteCount];

	// Read ptr reads from surface, starting value is first pixel.
	Uint8* readPtr = surfacePixels;

	// Write ptr writes to buffer, starting value is the first pixel outside of the image.
	Uint8* writePtr = pixelBuffer + w * h * pixelByteCount;

	// Cop pixels from surface to buffer until alls pixels are copied
	while(writePtr > surfacePixels) {
		writePtr -= (w * pixelByteCount); // Move write ptr back one image row
		std::memcpy(writePtr, readPtr, w * pixelByteCount); // Copy one image row to buffer
		readPtr += (w * pixelByteCount); // Move read ptr forward one image row
	}

	// Copy pixels back from buffer to surface and free buffer
	std::memcpy(surfacePixels, pixelBuffer, imageByteCount);
	delete[] pixelBuffer;
}

GLuint loadTexture(const std::string& path) noexcept
{
	// Load specified surface.
	SDL_Surface* surface = NULL;
	surface = IMG_Load(path.c_str());
	if (surface == NULL) {
		std::cerr << "Unable to load image at: " << path << ", error: " << IMG_GetError();
		std::terminate();
	}

	// Some error checking.
	if (surface->format->BytesPerPixel != 4) {
		std::cerr << "Image doesn't have 4 bytes per pixel." << std::endl;
		std::terminate();
	}
	if (!SDL_ISPIXELFORMAT_ALPHA(surface->format->format)) {
		std::cerr << "Image doesn't contain alpha channel." << std::endl;
		std::terminate();
	}

	// Flips surface so UV coordinates will be in a right-handed system in OpenGL.
	flipSurface(surface);

	// Creating OpenGL Texture from surface.
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);

	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Enable anisotropic filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	if (checkAllGLErrors()) {
		std::cerr << "^^^ Above errors likely caused by loading texture." << std::endl;
	}

	return texture;
}
	
} // namespace

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Texture::Texture(const std::string& path) noexcept
:
	mHandle(loadTexture(path))
{
	// Initialization complete.
}
	
Texture::~Texture() noexcept
{
	glDeleteTextures(1, &mHandle);
}
	
} // namespace gl
