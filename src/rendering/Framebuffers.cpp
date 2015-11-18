#include "rendering/Framebuffers.hpp"

#include <algorithm> // std::swap

#include <sfz/gl/OpenGL.hpp>

namespace s3 {

// GBuffer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GBuffer::GBuffer(vec2 dimensions) noexcept
:
	GBuffer{vec2i{(int)dimensions.x, (int)dimensions.y}}
{ }

GBuffer::GBuffer(vec2i dimensions) noexcept
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

	// Position texture
	glGenTextures(1, &mPositionTexture);
	glBindTexture(GL_TEXTURE_2D, mPositionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mDim.x, mDim.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPositionTexture, 0);

	// Normal texture
	glGenTextures(1, &mNormalTexture);
	glBindTexture(GL_TEXTURE_2D, mNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mDim.x, mDim.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormalTexture, 0);

	// Material Id texture
	glGenTextures(1, &mMaterialIdTexture);
	glBindTexture(GL_TEXTURE_2D, mMaterialIdTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, mDim.x, mDim.y, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mMaterialIdTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

	// Check that framebuffer is okay
	sfz_assert_release((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBuffer::GBuffer(GBuffer&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mDepthBuffer, other.mDepthBuffer);
	std::swap(this->mPositionTexture, other.mPositionTexture);
	std::swap(this->mNormalTexture, other.mNormalTexture);
	std::swap(this->mMaterialIdTexture, other.mMaterialIdTexture);
	std::swap(this->mDim, other.mDim);
}

GBuffer& GBuffer::operator= (GBuffer&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mDepthBuffer, other.mDepthBuffer);
	std::swap(this->mPositionTexture, other.mPositionTexture);
	std::swap(this->mNormalTexture, other.mNormalTexture);
	std::swap(this->mMaterialIdTexture, other.mMaterialIdTexture);
	std::swap(this->mDim, other.mDim);
	return *this;
}

GBuffer::~GBuffer() noexcept
{
	glDeleteTextures(1, &mDepthBuffer);
	glDeleteTextures(1, &mPositionTexture);
	glDeleteTextures(1, &mNormalTexture);
	glDeleteTextures(1, &mMaterialIdTexture);
	glDeleteFramebuffers(1, &mFBO);
}

} // namespace s3