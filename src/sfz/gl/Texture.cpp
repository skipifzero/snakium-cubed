#include "sfz/gl/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdint>
#include <cstring> // std::memcpy
#include <iostream>
#include <exception> // std::terminate
#include <algorithm> // std::swap

namespace gl {
	
namespace {

using std::uint8_t;

/*// Simple function that flips a surface by byte-level manipulation.
void flipSurface(SDL_Surface* surface) noexcept
{
	// Locking the surface
	if (SDL_LockSurface(surface) < 0) {
		std::cerr << "Couldn't lock surface for image flipping: " << SDL_GetError() << std::endl;
	}

	// Constants
	const int w = surface->w;
	const int h = surface->h;
	const int pixelCount = w * h;
	const int bytesPerPixel = surface->format->BytesPerPixel;
	const int bytesPerRow = w * bytesPerPixel;
	const int totalByteCount = pixelCount * bytesPerPixel;
	Uint32* const surfacePixels = static_cast<Uint32*>(surface->pixels);
	Uint32* const pixelBuffer = new Uint32[pixelCount];

	// surfPtr reads from surface, starting value is first pixel.
	Uint32* surfPtr = surfacePixels;

	// bufPtr writes to buffer, starting value is the first pixel outside the image
	Uint32* bufPtr = pixelBuffer + pixelCount;

	// Copy pixels from surface to buffer until all pixels are copied
	while (bufPtr > pixelBuffer) {
		bufPtr = bufPtr - w; // Move bufPtr back one image row
		std::memcpy(bufPtr, surfPtr, bytesPerRow); // Copy one image row to buffer
		surfPtr = surfPtr + w; // Move surfPtr forward one image row
	}

	// Copy pixels back from buffer and free memory.
	std::memcpy(surfacePixels, pixelBuffer, totalByteCount);
	delete[] pixelBuffer;

	// Unlocking the surface
	SDL_UnlockSurface(surface);
}*/

void flipImage(uint8_t* const pixels, int w, int h, int pitch, int numChannels)
{
	const int bytesPerRow = w*numChannels;
	uint8_t* const buffer = new uint8_t[bytesPerRow];

	for (int i = 0; i < (h/2); ++i) {
		uint8_t* begin = pixels + i*pitch;
		uint8_t* end = pixels + (h-i-1)*pitch;

		std::memcpy(buffer, begin, bytesPerRow);
		std::memcpy(begin, end, bytesPerRow);
		std::memcpy(end, buffer, bytesPerRow);
	}

	delete[] buffer;

	/*// Constants
	const int pixelCount = w * h;
	const int bytesPerPixel = 4;
	const int bytesPerRow = w * bytesPerPixel;
	const int totalByteCount = pixelCount * bytesPerPixel;
	uint8_t* const buffer = new uint8_t[pixelCount*bytesPerPixel];

	// surfPtr reads from surface, starting value is first pixel.
	Uint32* surfPtr = surfacePixels;

	// bufPtr writes to buffer, starting value is the first pixel outside the image
	Uint32* bufPtr = pixelBuffer + pixelCount;

	// Copy pixels from surface to buffer until all pixels are copied
	while (bufPtr > pixelBuffer) {
		bufPtr = bufPtr - w; // Move bufPtr back one image row
		std::memcpy(bufPtr, surfPtr, bytesPerRow); // Copy one image row to buffer
		surfPtr = surfPtr + w; // Move surfPtr forward one image row
	}

	// Copy pixels back from buffer and free memory.
	std::memcpy(surfacePixels, pixelBuffer, totalByteCount);
	delete[] buffer;*/
}

GLuint loadTexture(const string& path) noexcept
{
	// Loading image
	int width, height, numChannels;
	int numChannelsWanted = 0;
	uint8_t* img = stbi_load(path.c_str(), &width, &height, &numChannels, numChannelsWanted);

	// Some error checking
	if (img == NULL) {		
		std::cerr << "Unable to load image at: " << path << std::endl;
		std::terminate();
	}
	if (numChannels != 4) {
		std::cerr << "Number of channels in image not equal to 4 in inage at: " << path << std::endl;
		std::terminate();
	}

	// Flips image so UV coordinates will be in a right-handed system in OpenGL.
	flipImage(img, width, height, width, numChannels);

	/*// Load specified surface.
	SDL_Surface* surface = NULL;
	surface = IMG_Load(path.c_str());
	if (surface == NULL) {
		std::cerr << "Unable to load image at: " << path << ", error: " << IMG_GetError();
		std::terminate();
	}

	// Some error checking.
	if (surface->format->BytesPerPixel != 4) {
		std::cerr << "Image at \"" << path << "\" doesn't have 4 bytes per pixel." << std::endl;
		std::terminate();
	}
	if (!SDL_ISPIXELFORMAT_ALPHA(surface->format->format)) {
		std::cerr << "Image at \"" << path << "\" doesn't contain alpha channel." << std::endl;
		std::terminate();
	}

	// Flips surface so UV coordinates will be in a right-handed system in OpenGL.
	flipSurface(surface);*/

	// Creating OpenGL Texture from surface.
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, img);
	//SDL_FreeSurface(surface);
	stbi_image_free(img);

	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Enable anisotropic filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	if (checkAllGLErrors()) {
		std::cerr << "^^^ Above errors likely caused by loading texture at \"" << path
		          << "\"." << std::endl;
	}

	return texture;
}
	
} // namespace

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Texture::Texture(const string& path) noexcept
:
	mHandle(loadTexture(path))
{
	// Initialization complete.
}

Texture& Texture::operator= (Texture&& other) noexcept
{
	std::swap(mHandle, other.mHandle);
	return *this;
}

Texture::~Texture() noexcept
{
	glDeleteTextures(1, &mHandle);
}
	
} // namespace gl

