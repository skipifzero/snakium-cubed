#include "sfz/gl/TexturePacker.hpp"

#include <new> // std::nothrow
#include <cstring> // std::memcpy
#include <iostream>
#include <exception> // std::terminate
#include <algorithm> // std::swap

#include <SDL.h>

#include "sfz/PushWarnings.hpp"
//#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "sfz/PopWarnings.hpp"

#include "sfz/Assert.hpp"
#include "sfz/gl/GLUtils.hpp"
#include "sfz/gl/OpenGL.hpp"
#include "sfz/math/vector.hpp"

namespace gl {

using sfz::vec2;

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

static SDL_Surface* loadTexture(const string& path) noexcept
{
	// Loading image
	int width, height, numChannels;
	uint8_t* data = stbi_load(path.c_str(), &width, &height, &numChannels, 4);

	// Some error checking
	if (data == NULL) {		
		std::cerr << "Unable to load image at: " << path << ", reason: "
		          << stbi_failure_reason() << std::endl;
		std::terminate();
	}
	if (numChannels != 4)
	{
		std::cerr << "Number of channels in image not equal to 4 at: " << path << std::endl;
		std::terminate();
	}

	// Flips image so UV coordinates will be in a right-handed system in OpenGL.
	flipImage(data, width, height, width, numChannels);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const uint32_t rmask = 0xff000000;
	const uint32_t gmask = 0x00ff0000;
	const uint32_t bmask = 0x0000ff00;
	const uint32_t amask = 0x000000ff;
#else
	const uint32_t rmask = 0x000000ff;
	const uint32_t gmask = 0x0000ff00;
	const uint32_t bmask = 0x00ff0000;
	const uint32_t amask = 0xff000000;
#endif

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, width, height, 32, width*numChannels,
	                                                rmask, gmask, bmask, amask);

	if (surface == NULL) {
		std::cerr << "SDL_CreateRGBSurfaceFrom() failed for image at: " << path << ", SDL error: "
		          << SDL_GetError() << std::endl;
		std::terminate();
	}

	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

	return surface;
}

static bool packRects(vector<stbrp_rect>& rects, int width, int height) noexcept
{
	stbrp_context packContext;
	stbrp_node* nodes = new (std::nothrow) stbrp_node[width+2];
	stbrp_init_target(&packContext, width, height, nodes, width);
	stbrp_pack_rects(&packContext, rects.data(), rects.size());
	delete[] nodes;
	
	// Check if all rects were packed
	for (auto& rect : rects) {
		if (!rect.was_packed) return false;
	}
	return true;
}

// TexturePacker: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TexturePacker::TexturePacker(const string& dirPath, const vector<string>& filenames, int padding,
                             size_t suggestedWidth, size_t suggestedHeight,
                             TextureFiltering filtering) noexcept
:
	mWidth{suggestedWidth},
	mHeight{suggestedHeight},
	mFilenames(filenames)
{
	size_t size = filenames.size();

	// Loads surfaces and creates rects for packing
	vector<SDL_Surface*> surfaces;
	vector<stbrp_rect> rects;
	for (auto& filename : filenames) {
		surfaces.emplace_back(loadTexture(dirPath + filename));
		struct stbrp_rect r;
		r.id = surfaces.size()-1;
		r.w = surfaces.back()->w + 2*padding;
		r.h = surfaces.back()->h + 2*padding;
		rects.push_back(r);
	}

	// Increases size until packing succeeds
	bool widthIncTurn = true;
	while (!packRects(rects, (int)mWidth, (int)mHeight)) {
		if (widthIncTurn) mWidth *= 2;
		else mHeight *= 2;
		widthIncTurn = !widthIncTurn;
	}

	// Creates surface and makes sure it's empty.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const uint32_t rmask = 0xff000000;
	const uint32_t gmask = 0x00ff0000;
	const uint32_t bmask = 0x0000ff00;
	const uint32_t amask = 0x000000ff;
#else
	const uint32_t rmask = 0x000000ff;
	const uint32_t gmask = 0x0000ff00;
	const uint32_t bmask = 0x00ff0000;
	const uint32_t amask = 0xff000000;
#endif
	SDL_Surface* surface = SDL_CreateRGBSurface(0, mWidth, mHeight, 32, rmask, gmask, bmask, amask);
	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
	SDL_FillRect(surface, NULL, 0);

	// Blitting individual surfaces to common surface and calculating TextureRegions
	vec2 texDimInv{1.0f/(float)mWidth, 1.0f/(float)mHeight};
	for (size_t i = 0; i < size; ++i) {
		SDL_Rect dstRect;
		dstRect.w = surfaces[i]->w - 2*padding;
		dstRect.h = surfaces[i]->h - 2*padding;
		dstRect.x = rects[i].x + padding;
		dstRect.y = rects[i].y + padding;

		SDL_BlitSurface(surfaces[i], NULL, surface, &dstRect);

		// Calculate TextureRegion
		const vec2 offset{0.35f, 0.35f}; // Small hack to fix pixel imprecision
		vec2 min = (vec2{(float)(dstRect.x + padding), (float)(dstRect.y + padding)} - offset) * texDimInv;
		vec2 max = (vec2{(float)(dstRect.x + dstRect.w - padding), (float)(dstRect.y + dstRect.h - padding)} + offset) * texDimInv;
		mTextureRegionMap[filenames[i]] = TextureRegion{min, max};
	}

	// Cleaning up surfaces
	for (SDL_Surface* surface : surfaces) {
		uint8_t* data = (uint8_t*)surface->pixels;
		SDL_FreeSurface(surface);
		stbi_image_free(data);
	}

	// Generating texture
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	int numChannels = 4;
	int width = surface->w;
	int height = surface->h;

	switch (numChannels) {
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, surface->pixels);
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, surface->pixels);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		break;
	}
	SDL_FreeSurface(surface);

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
}

TexturePacker::~TexturePacker() noexcept
{
	glDeleteTextures(1, &mTexture);
}

// TexturePacker: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const TextureRegion* TexturePacker::textureRegion(const string& filename) const noexcept
{
	auto it = mTextureRegionMap.find(filename);
	if (it == mTextureRegionMap.end()) return nullptr;
	return &it->second;
}

} // namespace sfz