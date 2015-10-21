#include "rendering/Framebuffers.hpp"

#include <algorithm> // std::swap

#include <sfz/gl/OpenGL.hpp>

namespace s3 {

// ExternalFB: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ExternalFB::ExternalFB(vec2 dimensions) noexcept
:
	ExternalFB{vec2i{(int)dimensions.x, (int)dimensions.y}}
{ }

ExternalFB::ExternalFB(vec2i dimensions) noexcept
{
	sfz_assert_debug(dimensions.x > 0);
	sfz_assert_debug(dimensions.y > 0);
	mDim = dimensions;

	// Generate framebuffer
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// Depth buffer
	glGenTextures(1, &mDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, mDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mDim.x, mDim.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthBuffer, 0);

	// Color texture
	glGenTextures(1, &mColorTexture);
	glBindTexture(GL_TEXTURE_2D, mColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mDim.x, mDim.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);

	GLenum drawBuffers[] ={ GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

	// Check that framebuffer is okay
	sfz_assert_release((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ExternalFB::ExternalFB(ExternalFB&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mDepthBuffer, other.mDepthBuffer);
	std::swap(this->mColorTexture, other.mColorTexture);
	std::swap(this->mDim, other.mDim);
}

ExternalFB& ExternalFB::operator= (ExternalFB&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mDepthBuffer, other.mDepthBuffer);
	std::swap(this->mColorTexture, other.mColorTexture);
	std::swap(this->mDim, other.mDim);
	return *this;
}

ExternalFB::~ExternalFB() noexcept
{
	glDeleteTextures(1, &mDepthBuffer);
	glDeleteTextures(1, &mColorTexture);
	glDeleteFramebuffers(1, &mFBO);
}

// Post Process FBO
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

PostProcessFramebuffer::PostProcessFramebuffer(int width, int height) noexcept
:
	mWidth{width},
	mHeight{height}
{
	// Generate framebuffer
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// Color texture
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

	// Check that framebuffer is okay
	sfz_assert_release((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PostProcessFramebuffer::PostProcessFramebuffer(PostProcessFramebuffer&& other) noexcept
{
	glGenFramebuffers(1, &mFBO);
	glGenTextures(1, &mTexture);
	std::swap(mFBO, other.mFBO);
	std::swap(mTexture, other.mTexture);
	std::swap(mWidth, other.mWidth);
	std::swap(mHeight, other.mHeight);
}

PostProcessFramebuffer& PostProcessFramebuffer::operator= (PostProcessFramebuffer&& other) noexcept
{
	std::swap(mFBO, other.mFBO);
	std::swap(mTexture, other.mTexture);
	std::swap(mWidth, other.mWidth);
	std::swap(mHeight, other.mHeight);
	return *this;
}

PostProcessFramebuffer::~PostProcessFramebuffer() noexcept
{
	glDeleteTextures(1, &mTexture);
	glDeleteFramebuffers(1, &mFBO);
}

} // namespace s3