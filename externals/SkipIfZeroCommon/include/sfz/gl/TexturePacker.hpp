#pragma once
#ifndef SFZ_GL_TEXTURE_PACKER_HPP
#define SFZ_GL_TEXTURE_PACKER_HPP

#include <sfz/gl/Texture.hpp>
#include <sfz/gl/TextureEnums.hpp>
#include <sfz/gl/TextureRegion.hpp>

#include <cstddef> // size_t
#include <vector>
#include <string>
#include <unordered_map>

namespace gl {

using std::size_t;
using std::vector;
using std::string;
using std::uint32_t;
using std::unordered_map;

// TexturePacker
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class TexturePacker final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	TexturePacker() noexcept = delete;
	TexturePacker(const TexturePacker&) noexcept = delete;
	TexturePacker(TexturePacker&&) noexcept = delete;
	TexturePacker& operator= (const TexturePacker&) noexcept = delete;
	TexturePacker& operator= (TexturePacker&&) noexcept = delete;

	TexturePacker(const string& dirPath, const vector<string>& filenames, int padding = 1,
	              size_t suggestedWidth = 256, size_t suggestedHeight = 256,
	              TextureFiltering filtering = TextureFiltering::ANISOTROPIC_16) noexcept;
	~TexturePacker() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline uint32_t texture() const noexcept { return mTexture; }
	inline size_t textureWidth() const noexcept { return mWidth; }
	inline size_t textureHeight() const noexcept { return mHeight; }
	inline const vector<string>& filenames() const noexcept { return mFilenames; }
	const TextureRegion* textureRegion(const string& filename) const noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mTexture;
	size_t mWidth, mHeight;
	vector<string> mFilenames;
	unordered_map<string, TextureRegion> mTextureRegionMap;
};

} // namespace sfz
#endif