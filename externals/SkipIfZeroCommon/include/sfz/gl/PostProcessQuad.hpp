#pragma once
#ifndef VOX_FULLSCREEN_QUAD_OBJECT_HPP
#define VOX_FULLSCREEN_QUAD_OBJECT_HPP

#include <cstdint>

namespace gl {

using std::uint32_t;

class PostProcessQuad final {
public:	
	PostProcessQuad(const PostProcessQuad&) = delete;
	PostProcessQuad& operator= (const PostProcessQuad&) = delete;

	PostProcessQuad() noexcept;
	~PostProcessQuad() noexcept;

	void render() noexcept;

private:
	uint32_t mVAO = 0;
	uint32_t mPosBuffer = 0;
	uint32_t mNormalBuffer = 0;
	uint32_t mUVBuffer = 0;
	uint32_t mMaterialIDBuffer = 0;
	uint32_t mIndexBuffer = 0;
};

} // namespace gl
#endif