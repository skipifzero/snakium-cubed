#include "sfz/gl/FontRenderer.hpp"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb_truetype.h>

#include <sfz/gl/Utils.hpp>
#include <new> // std::nothrow
#include <cstdio>
#include <cstdlib> // malloc
#include <cstring> // std::memcpy
#include <iostream> // std::cerr
#include <exception> // std::terminate

namespace gl {

// Anonymous namespace
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {

// Anonymous: UTF8 decoder
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

static const uint8_t utf8d[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
	8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
	0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
	0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
	0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
	1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
	1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
	1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

bool decode(uint32_t* state, uint32_t* codep, uint32_t byte)
{
	uint32_t type = utf8d[byte];
	*codep = (*state != 0) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
	*state = utf8d[256 + *state*16 + type];
	return *state;
}

// Anonymous: Shaders
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const char* FONT_RENDERER_FRAGMENT_SHADER_SRC = R"(
	#version 330

	precision highp float; // required by GLSL spec Sect 4.5.3

	// Input
	in vec2 uvCoord;

	// Output
	out vec4 fragmentColor;

	// Uniforms
	uniform sampler2D uTexture;
	uniform vec4 uTextColor;

	void main()
	{
		fragmentColor = uTextColor * texture(uTexture, uvCoord).r;
	}
)";

// Anonymous: Functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

uint8_t* loadTTFBuffer(const std::string& path) noexcept
{
	const size_t MAX_TTF_BUFFER_SIZE = 1<<22; // 4 MiB
	uint8_t* buffer = new (std::nothrow) uint8_t[MAX_TTF_BUFFER_SIZE];

	std::FILE* ttfFile = std::fopen(path.c_str(), "rb");
	if (ttfFile == NULL) {
		std::cerr << "Couldn't open TTF file at: " << path << std::endl;
		std::terminate();
	}

	size_t readCount = std::fread(buffer, sizeof(uint8_t), MAX_TTF_BUFFER_SIZE, ttfFile);
	if (readCount == 0) {
		std::cerr << "Loaded no bytes from TTF file at: " << path << std::endl;
		std::terminate();
	}

	std::fclose(ttfFile);
	return buffer;
}

struct CharInfo {
	vec2 pos;
	vec2 dim;
	TextureRegion texRegion;
};

void calculateCharInfo(CharInfo& info, void* chardata, vec2 pixelToUV, uint32_t codeIndex,
                       vec2& pos, float scale) noexcept
{
	stbtt_packedchar& c = reinterpret_cast<stbtt_packedchar*>(chardata)[codeIndex];

	info.dim = vec2{c.xoff2 - c.xoff, c.yoff2 - c.yoff} * scale;
	info.texRegion.mUVMin = vec2{(float)c.x0, (float)c.y1} * pixelToUV[0];
	info.texRegion.mUVMax = vec2{(float)c.x1, (float)c.y0} * pixelToUV[1];

	info.pos[0] = pos[0] + info.dim[0]/2.0f;
	info.pos[1] = pos[1] - ((c.yoff + c.yoff2) / 2.0f) * scale;

	pos[0] += c.xadvance * scale;
}

} // anonymous namespace

// FontRenderer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

FontRenderer::FontRenderer(const std::string& fontPath, uint32_t texWidth, uint32_t texHeight,
	                       float fontSize, size_t numCharsPerBatch) noexcept
:
	mFontSize{fontSize},
	mPackedChars{new (std::nothrow) stbtt_packedchar[CHAR_COUNT]},
	mSpriteBatch{numCharsPerBatch, FONT_RENDERER_FRAGMENT_SHADER_SRC}
{
	// This should be const, but MSVC12 doesn't support ini lists in constructor's ini list
	mPixelToUV = vec2{1.0f/static_cast<float>(texWidth), 1.0f/static_cast<float>(texHeight)};

	uint8_t* tempBitmap = new uint8_t[texWidth*texHeight];

	stbtt_pack_context packContext;
	if(stbtt_PackBegin(&packContext, tempBitmap, texWidth, texHeight, 0, 1, NULL) == 0) {
		std::cerr << "FontRenderer: Couldn't stbtt_PackBegin()" << std::endl;
		std::terminate();
	}

	stbtt_PackSetOversampling(&packContext, 2, 2);

	uint8_t* ttfBuffer = loadTTFBuffer(fontPath);

	if (stbtt_PackFontRange(&packContext, ttfBuffer, 0, mFontSize, FIRST_CHAR, CHAR_COUNT,
	                    reinterpret_cast<stbtt_packedchar*>(mPackedChars)) == 0) {
		std::cerr << "FontRenderer: Couldn't pack font, texture likely too small." << std::endl;
		std::terminate();
	}

	stbtt_PackEnd(&packContext);
	delete[] ttfBuffer;

	glGenTextures(1, &mFontTexture);
	glBindTexture(GL_TEXTURE_2D, mFontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texWidth, texHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
	             tempBitmap);
	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Enable anisotropic filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	delete[] tempBitmap;
}

FontRenderer::~FontRenderer() noexcept
{
	glDeleteTextures(1, &mFontTexture);
	delete[] reinterpret_cast<stbtt_packedchar* const>(mPackedChars);
}

// FontRenderer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void FontRenderer::begin(vec2 cameraPosition, vec2 cameraDimensions) noexcept
{
	mSpriteBatch.begin(cameraPosition, cameraDimensions);
}

float FontRenderer::write(vec2 position, float size, const std::string& text) noexcept
{
	const float scale = size / mFontSize;

	vec2 currPos = position;
	const float vertOffsetScale = distance(VerticalAlign::BOTTOM, mVertAlign) / 2.0f;
	currPos[1] -= size * vertOffsetScale;
	if (mHorizAlign != HorizontalAlign::LEFT) {
		const float strWidth = measureStringWidth(size, text);
		const float horizOffsetScale = distance(HorizontalAlign::LEFT, mHorizAlign) / 2.0f;
		currPos[0] -= strWidth * horizOffsetScale;
	}

	CharInfo info;
	uint32_t codepoint;
	uint32_t state = 0;
	for (uint8_t c : text) {
		if (decode(&state, &codepoint, c)) continue;
		codepoint -= FIRST_CHAR;
		if (LAST_CHAR < codepoint) codepoint = UNKNOWN_CHAR - FIRST_CHAR;

		calculateCharInfo(info, mPackedChars, mPixelToUV, codepoint, currPos, scale);
		mSpriteBatch.draw(info.pos, info.dim, info.texRegion);
	}

	return currPos[0];
}

void FontRenderer::writeBitmapFont(vec2 position, vec2 dimensions) noexcept
{
	mSpriteBatch.draw(position, dimensions, TextureRegion{vec2{0.0f, 0.0f}, vec2{1.0f, 1.0f}});
}

void FontRenderer::end(GLuint fbo, vec2 viewportDimensions, vec4 textColor) noexcept
{
	glUseProgram(mSpriteBatch.shaderProgram());
	gl::setUniform(mSpriteBatch.shaderProgram(), "uTextColor", textColor);
	mSpriteBatch.end(fbo, viewportDimensions, mFontTexture);
}

float FontRenderer::measureStringWidth(float size, const std::string& text) const noexcept
{
	const float scale = size / mFontSize;
	CharInfo info;
	vec2 currPos = vec2{0.0f};
	uint32_t codepoint;
	uint32_t state = 0;
	for (uint8_t c : text) {
		if (decode(&state, &codepoint, c)) continue;
		codepoint -= FIRST_CHAR;
		if (LAST_CHAR < codepoint) codepoint = UNKNOWN_CHAR - FIRST_CHAR;
		calculateCharInfo(info, mPackedChars, mPixelToUV, codepoint, currPos, scale);
	}
	return currPos[0];
}

} // namespace sfz

