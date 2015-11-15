#include "sfz/gl/PostProcessFB.hpp"

#include "sfz/gl/OpenGL.hpp"

namespace gl {

// PostProcessFB: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

PostProcessFB::PostProcessFB(vec2i dimensions, TextureFormat texFormat) noexcept
{
	sfz_assert_debug(dimensions.x > 0);
	sfz_assert_debug(dimensions.y > 0);
	mDim = dimensions;

	// Generate framebuffer
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// Color texture
	glGenTextures(1, &mColorTexture);
	glBindTexture(GL_TEXTURE_2D, mColorTexture);
	switch (texFormat) {
	case TextureFormat::GRAY:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mDim.x, mDim.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::GRAY_ALPHA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, mDim.x, mDim.y, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mDim.x, mDim.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::RGBA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mDim.x, mDim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

	// Check that framebuffer is okay
	sfz_assert_release((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PostProcessFB::PostProcessFB(int32_t width, int32_t height, TextureFormat texFormat) noexcept
:
	PostProcessFB{vec2i{width, height}, texFormat}
{ }

PostProcessFB::PostProcessFB(PostProcessFB&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mColorTexture, other.mColorTexture);
	std::swap(this->mDim, other.mDim);
}

PostProcessFB& PostProcessFB::operator= (PostProcessFB&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mColorTexture, other.mColorTexture);
	std::swap(this->mDim, other.mDim);
	return *this;
}

PostProcessFB::~PostProcessFB() noexcept
{
	glDeleteTextures(1, &mColorTexture);
	glDeleteFramebuffers(1, &mFBO);
}

} // namespace gl