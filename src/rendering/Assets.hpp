#pragma once
#ifndef S3_ASSETS_HPP
#define S3_ASSETS_HPP

#include <sfz/GL.hpp>

#include <sfz/gl/SimpleModel.hpp> // TODO: Temp

#include <sfz/sdl/Music.hpp>
#include <sfz/sdl/SoundEffect.hpp>

namespace s3 {

using gl::Texture;
using sdl::Music;
using sdl::SoundEffect;

// Assets class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Assets final {
public:
	// Singleton instance
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static Assets& INSTANCE() noexcept;
	static void load() noexcept;
	static void destroy() noexcept;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gl::SpriteBatch spriteBatch;
	gl::FontRenderer fontRenderer;

	Texture HEAD_D2U_F1,
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

	Texture SNAKIUM_LOGO,
	        CREDITS_LOGO;

	gl::SimpleModel HEAD_D2U_F1_MODEL,
	                HEAD_D2U_F1_PROJECTION_MODEL,
	                HEAD_D2U_DIG_F1_MODEL,
	                HEAD_D2U_F2_MODEL,
	                HEAD_D2U_F2_PROJECTION_MODEL,
	                HEAD_D2U_DIG_F2_MODEL,

	                PRE_HEAD_D2U_F1_MODEL,
	                PRE_HEAD_D2U_F1_PROJECTION_MODEL,
	                PRE_HEAD_D2U_DIG_F1_MODEL,
	                PRE_HEAD_D2U_DIG_F1_PROJECTION_MODEL,
	                PRE_HEAD_D2R_F1_MODEL,
	                PRE_HEAD_D2R_F1_PROJECTION_MODEL,
	                PRE_HEAD_D2R_DIG_F1_MODEL,
	                PRE_HEAD_D2R_DIG_F1_PROJECTION_MODEL,
	                PRE_HEAD_D2L_F1_MODEL,
	                PRE_HEAD_D2L_F1_PROJECTION_MODEL,
	                PRE_HEAD_D2L_DIG_F1_MODEL,
	                PRE_HEAD_D2L_DIG_F1_PROJECTION_MODEL,

	                DEAD_PRE_HEAD_D2U_F1_MODEL,
	                DEAD_PRE_HEAD_D2U_DIG_F1_MODEL,
	                DEAD_PRE_HEAD_D2R_F1_MODEL,
	                DEAD_PRE_HEAD_D2R_DIG_F1_MODEL,
	                DEAD_PRE_HEAD_D2L_F1_MODEL,
	                DEAD_PRE_HEAD_D2L_DIG_F1_MODEL,

	                BODY_D2U_MODEL,
	                BODY_D2U_PROJECTION_MODEL,
	                BODY_D2U_DIG_MODEL,
	                BODY_D2U_DIG_PROJECTION_MODEL,
	                BODY_D2R_MODEL,
	                BODY_D2R_PROJECTION_MODEL,
	                BODY_D2R_DIG_MODEL,
	                BODY_D2R_DIG_PROJECTION_MODEL,
	                BODY_D2L_MODEL,
	                BODY_D2L_PROJECTION_MODEL,
	                BODY_D2L_DIG_MODEL,
	                BODY_D2L_DIG_PROJECTION_MODEL,

	                TAIL_D2U_F1_MODEL,
	                TAIL_D2U_F1_PROJECTION_MODEL,
	                TAIL_D2U_DIG_F1_MODEL,
	                TAIL_D2U_DIG_F1_PROJECTION_MODEL,
	                TAIL_D2U_F2_MODEL,
	                TAIL_D2U_F2_PROJECTION_MODEL,
	                TAIL_D2U_DIG_F2_MODEL,
	                TAIL_D2U_DIG_F2_PROJECTION_MODEL,
	                TAIL_D2R_F1_MODEL,
	                TAIL_D2R_F1_PROJECTION_MODEL,
	                TAIL_D2R_DIG_F1_MODEL,
	                TAIL_D2R_DIG_F1_PROJECTION_MODEL,
	                TAIL_D2R_F2_MODEL,
	                TAIL_D2R_F2_PROJECTION_MODEL,
	                TAIL_D2R_DIG_F2_MODEL,
	                TAIL_D2R_DIG_F2_PROJECTION_MODEL,
	                TAIL_D2L_F1_MODEL,
	                TAIL_D2L_F1_PROJECTION_MODEL,
	                TAIL_D2L_DIG_F1_MODEL,
	                TAIL_D2L_DIG_F1_PROJECTION_MODEL,
	                TAIL_D2L_F2_MODEL,
	                TAIL_D2L_F2_PROJECTION_MODEL,
	                TAIL_D2L_DIG_F2_MODEL,
	                TAIL_D2L_DIG_F2_PROJECTION_MODEL,

	                DIVE_MODEL,
	                ASCEND_MODEL,

	                OBJECT_PART1_MODEL,
	                OBJECT_PART2_MODEL,
	                OBJECT_PART3_MODEL,
	                OBJECT_PART4_MODEL,
	                BONUS_OBJECT_MODEL,

	                TILE_DECORATION_MODEL,
	                TILE_PROJECTION_MODEL,
	
	                SKYSPHERE_MODEL,
	                GROUND_MODEL,

	                NOT_FOUND_MODEL;

	Music GAME_MUSIC;

	SoundEffect GAME_OVER_SFX,
	            SHIFT_INITIATED_SFX,
	            SHIFT_ASCEND_SFX,

	            OBJECT_EATEN_LATE_SFX,
	            OBJECT_EATEN_LATE_SHIFT_SFX,
	            OBJECT_EATEN_SFX,
	            OBJECT_EATEN_SHIFT_SFX,

	            BONUS_OBJECT_ADDED_SFX,
	            BONUS_OBJECT_EATEN_SFX,
	            BONUS_OBJECT_EATEN_SHIFT_SFX,
	            BONUS_OBJECT_MISSED_SFX,
	
	            MENU_BUTTON_SELECTED_SFX;

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Assets(const Assets&) = delete;
	Assets& operator= (const Assets&) = delete;

	Assets() noexcept;
	~Assets() noexcept;
};

} // namespace s3

#endif