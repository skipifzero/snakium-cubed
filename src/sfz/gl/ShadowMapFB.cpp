#include "sfz/gl/ShadowMapFB.hpp"

#include "sfz/gl/OpenGL.hpp"

namespace sfz {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static GLint getDepth(ShadowMapDepthRes depthRes) noexcept
{
	switch (depthRes) {
	case ShadowMapDepthRes::BITS_16: return GL_DEPTH_COMPONENT16;
	case ShadowMapDepthRes::BITS_24: return GL_DEPTH_COMPONENT24;
	case ShadowMapDepthRes::BITS_32: return GL_DEPTH_COMPONENT32;
	}
	sfz_assert_debug(false);
}

// ShadowMapFB: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ShadowMapFB::ShadowMapFB(vec2i resolution, ShadowMapDepthRes depthRes, bool pcf, vec4 borderColor) noexcept
:
	mResolution{resolution},
	mHasPCF{pcf}
{
	// Generate framebuffer
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// Generates depth texture
	glGenTextures(1, &mDepthTexture);
	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, getDepth(depthRes), resolution.x, resolution.y, 0,
	             GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Set shadowmap texture min & mag filters (enable/disable pcf)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pcf ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pcf ? GL_LINEAR : GL_NEAREST);

	// Set texture wrap mode to CLAMP_TO_BORDER and set border color.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.elements);

	// Magic lines that enable hardware shadowmaps somehow (becomes sampler2Dshadow?)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	// Bind texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
	glDrawBuffer(GL_NONE); // No color buffer
	glReadBuffer(GL_NONE);

	// Check that framebuffer is okay
	sfz_assert_release((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMapFB::ShadowMapFB(vec2 resolution, ShadowMapDepthRes depthRes, bool pcf, vec4 borderColor) noexcept
:
	ShadowMapFB{vec2i{(int)resolution.x, (int)resolution.y}, depthRes, pcf, borderColor}
{ }

ShadowMapFB::ShadowMapFB(ShadowMapFB&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mDepthTexture, other.mDepthTexture);
	std::swap(this->mHasPCF, other.mHasPCF);
	std::swap(this->mResolution, other.mResolution);
}

ShadowMapFB& ShadowMapFB::operator= (ShadowMapFB&& other) noexcept
{
	std::swap(this->mFBO, other.mFBO);
	std::swap(this->mDepthTexture, other.mDepthTexture);
	std::swap(this->mHasPCF, other.mHasPCF);
	std::swap(this->mResolution, other.mResolution);
	return *this;
}

ShadowMapFB::~ShadowMapFB() noexcept
{
	glDeleteTextures(1, &mDepthTexture);
	glDeleteFramebuffers(1, &mFBO);
}

} // namespace sfz