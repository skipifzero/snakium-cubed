#include "sfz/gl/Texture.hpp"

#include <sfz/PushWarnings.hpp>
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <sfz/PopWarnings.hpp>

#include <algorithm> // std::swap
#include <cstring> // std::memcpy
#include <exception> // std::terminate
#include <iostream>
#include <new>

namespace gl {
	
namespace {

void flipImage(uint8_t* const pixels, int w, int h, int pitch, int numChannels) noexcept
{
	const int bytesPerRow = w*numChannels;
	const int bytePitch = pitch*numChannels;
	uint8_t* const buffer = new (std::nothrow) uint8_t[bytesPerRow];

	for (int i = 0; i < (h/2); ++i) {
		uint8_t* begin = pixels + i*bytePitch;
		uint8_t* end = pixels + (h-i-1)*bytePitch;

		std::memcpy(buffer, begin, bytesPerRow);
		std::memcpy(begin, end, bytesPerRow);
		std::memcpy(end, buffer, bytesPerRow);
	}

	delete[] buffer;
}

GLuint loadTexture(const string& path, int numChannelsWanted) noexcept
{
	// Loading image
	int width, height, numChannels;
	uint8_t* img = stbi_load(path.c_str(), &width, &height, &numChannels, numChannelsWanted);

	// Some error checking
	if (img == NULL) {		
		std::cerr << "Unable to load image at: " << path << ", reason: "
		          << stbi_failure_reason() << std::endl;
		std::terminate();
	}

	// Flips image so UV coordinates will be in a right-handed system in OpenGL.
	flipImage(img, width, height, width, numChannels);

	// Creating OpenGL Texture from surface.
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	switch (numChannels) {
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, img);
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, img);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		break;
	}
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

Texture::Texture(const string& path, TextureFormat format) noexcept
:
	mHandle{loadTexture(path, static_cast<uint8_t>(format))}
{ }

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

