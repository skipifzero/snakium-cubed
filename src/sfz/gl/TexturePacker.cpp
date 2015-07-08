#include "sfz/gl/TexturePacker.hpp"

//#define STB_RECT_PACK_IMPLEMENTATION
#include "sfz/gl/font/stb_rect_pack.h"

#include "sfz/gl/Utils.hpp"
#include <SDL_image.h>
#include <new> // std::nothrow
#include <cstring> // std::memcpy
#include <iostream>
#include <exception> // std::terminate
#include <algorithm> // std::swap

namespace gl {

// Anonymous namespace
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {

// Simple function that flips a surface by byte-level manipulation.
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
}

SDL_Surface* loadTexture(const std::string& path) noexcept
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
		std::cerr << "Image at \"" << path << "\" doesn't have 4 bytes per pixel." << std::endl;
		std::terminate();
	}
	if (!SDL_ISPIXELFORMAT_ALPHA(surface->format->format)) {
		std::cerr << "Image at \"" << path << "\" doesn't contain alpha channel." << std::endl;
		std::terminate();
	}

	// Flips surface so UV coordinates will be in a right-handed system in OpenGL.
	flipSurface(surface);

	return surface;
}

bool packRects(vector<stbrp_rect>& rects, int width, int height) noexcept
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

} // anonymous namespace

// TexturePacker: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TexturePacker::TexturePacker(const string& dirPath, const vector<string>& filenames, int padding,
                             size_t suggestedWidth, size_t suggestedHeight) noexcept
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
	while (!packRects(rects, (int)mWidth, (int)mHeight))
	{
		if (widthIncTurn) mWidth *= 2;
		else mHeight *= 2;
		widthIncTurn = !widthIncTurn;
	}

	// Creates surface and makes sure it's empty.
	uint32_t rmask = 0x000000ff;
    uint32_t gmask = 0x0000ff00;
    uint32_t bmask = 0x00ff0000;
    uint32_t amask = 0xff000000;
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
		vec2 min = sfz::elemMult(vec2{(float)(dstRect.x + padding), (float)(dstRect.y + padding)}, texDimInv);
		vec2 max = sfz::elemMult(vec2{(float)(dstRect.x + dstRect.w - 2*padding), (float)(dstRect.y + dstRect.h - 2*padding)}, texDimInv);
		mTextureRegionMap[filenames[i]] = TextureRegion{min, max};
	}

	// Cleaning up surfaces
	for (SDL_Surface* surface : surfaces) {
		SDL_FreeSurface(surface);
	}

	// Generating texture.
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
	             surface->pixels);
	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Enable anisotropic filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	SDL_FreeSurface(surface);
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