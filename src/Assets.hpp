#pragma once
#ifndef S3_ASSETS_HPP
#define S3_ASSETS_HPP

#include <sfz/SDL.hpp>
#include <sfz/GL.hpp>
#include <string>
#include "GameLogic.hpp"

namespace s3 {

// Assets class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Assets final {
public:
	// Singleton instance
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static Assets& INSTANCE() noexcept;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gl::SpriteBatch mSpriteBatch;
	gl::FontRenderer mFontRenderer;

	gl::Texture HEAD_D2U_F1,
	            HEAD_D2U_F2,

	            PRE_HEAD_D2U_F1,
	            PRE_HEAD_D2U_DIG_F1,
	            PRE_HEAD_D2R_F1,
	            PRE_HEAD_D2R_DIG_F1,

	            DEAD_PRE_HEAD_D2U_F1,
	            DEAD_PRE_HEAD_D2U_DIG_F1,
	            DEAD_PRE_HEAD_D2R_F1,
	            DEAD_PRE_HEAD_D2R_DIG_F1,

	            BODY_D2U,
	            BODY_D2U_DIG,
	            BODY_D2R,
	            BODY_D2R_DIG,

	            TAIL_D2U_F1,
	            TAIL_D2U_F2,
	            TAIL_D2U_DIG_F1,
	            TAIL_D2U_DIG_F2,
	            TAIL_D2R_F1,
	            TAIL_D2R_F2,
	            TAIL_D2R_DIG_F1,
	            TAIL_D2R_DIG_F2,

	            BUTTON_LEFT,
	            BUTTON_LEFT_TOUCHED,
	            BUTTON_LEFT_DISABLED,
	            BUTTON_MIDDLE_TOUCHED,
	            BUTTON_RIGHT,
	            BUTTON_RIGHT_TOUCHED,
	            BUTTON_RIGHT_DISABLED,

	            OBJECT,
	            BONUS_OBJECT,
	            FILLED,
	            TILE_FACE;
	
	gl::TexturePacker ATLAS_128;
	gl::TextureRegion HEAD_D2U_F1_REG,
	                  HEAD_D2U_F2_REG,

	                  PRE_HEAD_D2U_F1_REG,
	                  PRE_HEAD_D2U_DIG_F1_REG,
	                  PRE_HEAD_D2R_F1_REG,
	                  PRE_HEAD_D2R_DIG_F1_REG,

	                  DEAD_PRE_HEAD_D2U_F1_REG,
	                  DEAD_PRE_HEAD_D2U_DIG_F1_REG,
	                  DEAD_PRE_HEAD_D2R_F1_REG,
	                  DEAD_PRE_HEAD_D2R_DIG_F1_REG,

	                  BODY_D2U_REG,
	                  BODY_D2U_DIG_REG,
	                  BODY_D2R_REG,
	                  BODY_D2R_DIG_REG,

	                  TAIL_D2U_F1_REG,
	                  TAIL_D2U_F2_REG,
	                  TAIL_D2U_DIG_F1_REG,
	                  TAIL_D2U_DIG_F2_REG,
	                  TAIL_D2R_F1_REG,
	                  TAIL_D2R_F2_REG,
	                  TAIL_D2R_DIG_F1_REG,
	                  TAIL_D2R_DIG_F2_REG,

	                  BUTTON_LEFT_REG,
	                  BUTTON_LEFT_TOUCHED_REG,
	                  BUTTON_LEFT_DISABLED_REG,
	                  BUTTON_MIDDLE_TOUCHED_REG,
	                  BUTTON_RIGHT_REG,
	                  BUTTON_RIGHT_TOUCHED_REG,
	                  BUTTON_RIGHT_DISABLED_REG,

	                  OBJECT_REG,
	                  BONUS_OBJECT_REG,
	                  FILLED_REG,
	                  TILE_FACE_REG;

	gl::TexturePacker ATLAS_1024;
	gl::TextureRegion SNAKIUM_LOGO_REG,
	                  SKIPIFZERO_LOGO_REG,
	                  SKIPIFZERO_LOGO_SNAKIUM_VER_REG,
	                  COFFER_LOGO_REG;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	const gl::Texture& getTileTexture(SnakeTile* tilePtr, float progress, bool gameOver) const noexcept;

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Assets(const Assets&) = delete;
	Assets& operator= (const Assets&) = delete;

	Assets() noexcept;
};

} // namespace s3

#endif