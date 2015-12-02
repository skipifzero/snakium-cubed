#include "sfz/gl/PostProcessFB.hpp"

#include "sfz/gl/OpenGL.hpp"

namespace gl {

// PostProcessFB: Constructors functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

PostProcessFB PostProcessFB::createColor(vec2i dimensions,
                                         TextureFormat colorTexFormat) noexcept
{
	sfz_assert_debug(dimensions.x > 0);
	sfz_assert_debug(dimensions.y > 0);

	PostProcessFB tmp;
	tmp.mDim = dimensions;

	// Generate framebuffer
	glGenFramebuffers(1, &tmp.mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, tmp.mFBO);

	// Color texture
	glGenTextures(1, &tmp.mColorTexture);
	glBindTexture(GL_TEXTURE_2D, tmp.mColorTexture);
	switch (colorTexFormat) {
	case TextureFormat::GRAY:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, tmp.mDim.x, tmp.mDim.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::GRAY_ALPHA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, tmp.mDim.x, tmp.mDim.y, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, tmp.mDim.x, tmp.mDim.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::RGBA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tmp.mDim.x, tmp.mDim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmp.mColorTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

	// Check that framebuffer is okay
	sfz_assert_release((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::move(tmp);
}

PostProcessFB PostProcessFB::createColorStencil(vec2i dimensions,
                                                TextureFormat colorTexFormat) noexcept
{
	sfz_assert_debug(dimensions.x > 0);
	sfz_assert_debug(dimensions.y > 0);

	PostProcessFB tmp;
	tmp.mDim = dimensions;

	// Generate framebuffer
	glGenFramebuffers(1, &tmp.mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, tmp.mFBO);

	// Color texture
	glGenTextures(1, &tmp.mColorTexture);
	glBindTexture(GL_TEXTURE_2D, tmp.mColorTexture);
	switch (colorTexFormat) {
	case TextureFormat::GRAY:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, tmp.mDim.x, tmp.mDim.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::GRAY_ALPHA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, tmp.mDim.x, tmp.mDim.y, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, tmp.mDim.x, tmp.mDim.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		break;
	case TextureFormat::RGBA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tmp.mDim.x, tmp.mDim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmp.mColorTexture, 0);

	// Stencil buffer
	glGenRenderbuffers(1, &tmp.mStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, tmp.mStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, tmp.mDim.x, tmp.mDim.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tmp.mStencilBuffer);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

	// Check that framebuffer is okay
	sfz_assert_release((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::move(tmp);
}

// PostProcessFB: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

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
	glDeleteRenderbuffers(1, &mStencilBuffer);
	glDeleteFramebuffers(1, &mFBO);
}

} // namespace gl