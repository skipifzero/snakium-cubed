#include "sfz/gl/Texture.hpp"

#include <sfz/PushWarnings.hpp>
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <sfz/PopWarnings.hpp>

#include "sfz/Assert.hpp"
#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/GLUtils.hpp"

#include <algorithm> // std::swap
#include <cstring> // std::memcpy
#include <iostream>
#include <new>

namespace gl {

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static void flipImage(uint8_t* const pixels, int w, int h, int pitch, int numChannels) noexcept
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

static float anisotropicFactor(TextureFiltering filtering) noexcept
{
	switch (filtering) {
	case TextureFiltering::ANISOTROPIC_1: return 1.0f;
	case TextureFiltering::ANISOTROPIC_2: return 2.0f;
	case TextureFiltering::ANISOTROPIC_4: return 4.0f;
	case TextureFiltering::ANISOTROPIC_8: return 8.0f;
	case TextureFiltering::ANISOTROPIC_16: return 16.0f;
	default:
		sfz_error("Can't extract anisotropic factor.");
	}
}

static GLuint loadTexture(const char* path, int numChannelsWanted, TextureFiltering filtering) noexcept
{
	// Loading image
	int width, height, numChannels;
	uint8_t* img = stbi_load(path, &width, &height, &numChannels, numChannelsWanted);

	// Some error checking
	if (img == NULL) {		
		std::cerr << "Unable to load image at: " << path << ", reason: "
		          << stbi_failure_reason() << std::endl;
		return 0;
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

	// Sets specified texture filtering, generating mipmaps if needed.
	switch (filtering) {
	case TextureFiltering::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case TextureFiltering::BILINEAR:
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TextureFiltering::TRILINEAR:
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TextureFiltering::ANISOTROPIC_1:
	case TextureFiltering::ANISOTROPIC_2:
	case TextureFiltering::ANISOTROPIC_4:
	case TextureFiltering::ANISOTROPIC_8:
	case TextureFiltering::ANISOTROPIC_16:
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicFactor(filtering));
		break;
	}

	if (checkAllGLErrors()) {
		std::cerr << "^^^ Above errors likely caused by loading texture at \"" << path
		          << "\"." << std::endl;
	}

	return texture;
}

// Texture: Constructor functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Texture Texture::fromFile(const char* path, TextureFormat format, TextureFiltering filtering) noexcept
{
	Texture tmp;
	tmp.mHandle = loadTexture(path, static_cast<uint8_t>(format), filtering);
	return std::move(tmp);
}

// Texture: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Texture::Texture(Texture&& other) noexcept
{
	std::swap(this->mHandle, other.mHandle);
}

Texture& Texture::operator= (Texture&& other) noexcept
{
	std::swap(this->mHandle, other.mHandle);
	return *this;
}

Texture::~Texture() noexcept
{
	glDeleteTextures(1, &mHandle); // Silently ignores mHandle == 0
}
	
} // namespace gl

