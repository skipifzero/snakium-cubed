#include "Assets.hpp"

#include <string>

namespace s3 {

using std::string;

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const string& basePath() noexcept
{
	static const string BASE_PATH{SDL_GetBasePath()};
	return BASE_PATH;
}

static const string& assetsPath() noexcept
{
	static const string ASSETS_PATH{basePath() + "assets/"};
	return ASSETS_PATH;
}

static const string& snakeTexturePath() noexcept
{
	static const string SNAKE_TEXTURE_PATH{assetsPath() + "128pix/"};
	return SNAKE_TEXTURE_PATH;
}

static const char* modelPath() noexcept
{
	static const string path = assetsPath() + "models/";
	return path.c_str();
}

// Assets: Singleton instance
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Assets& Assets::INSTANCE() noexcept
{
	static Assets instance;
	return instance;
}

// Assets: Private constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Assets::Assets() noexcept
:
	spriteBatch{3000},
	fontRenderer{assetsPath() + "fonts/SaniTrixieSans.ttf", 2048, 2048, 125.0f, 3000},


	HEAD_D2U_F1{Texture::fromFile((snakeTexturePath() + "head_d2u_f1_128.png").c_str())},
	HEAD_D2U_F2{Texture::fromFile((snakeTexturePath() + "head_d2u_f2_128.png").c_str())},

	PRE_HEAD_D2U_F1{Texture::fromFile((snakeTexturePath() + "pre_head_d2u_f1_128.png").c_str())},
	PRE_HEAD_D2U_DIG_F1{Texture::fromFile((snakeTexturePath() + "pre_head_d2u_dig_f1_128.png").c_str())},
	PRE_HEAD_D2R_F1{Texture::fromFile((snakeTexturePath() + "pre_head_d2r_f1_128.png").c_str())},
	PRE_HEAD_D2R_DIG_F1{Texture::fromFile((snakeTexturePath() + "pre_head_d2r_dig_f1_128.png").c_str())},

	DEAD_PRE_HEAD_D2U_F1{Texture::fromFile((snakeTexturePath() + "dead_pre_head_d2u_f1_128.png").c_str())},
	DEAD_PRE_HEAD_D2U_DIG_F1{Texture::fromFile((snakeTexturePath() + "dead_pre_head_d2u_dig_f1_128.png").c_str())},
	DEAD_PRE_HEAD_D2R_F1{Texture::fromFile((snakeTexturePath() + "dead_pre_head_d2r_f1_128.png").c_str())},
	DEAD_PRE_HEAD_D2R_DIG_F1{Texture::fromFile((snakeTexturePath() + "dead_pre_head_d2r_dig_f1_128.png").c_str())},

	BODY_D2U{Texture::fromFile((snakeTexturePath() + "body_d2u_128.png").c_str())},
	BODY_D2U_DIG{Texture::fromFile((snakeTexturePath() + "body_d2u_dig_128.png").c_str())},
	BODY_D2R{Texture::fromFile((snakeTexturePath() + "body_d2r_128.png").c_str())},
	BODY_D2R_DIG{Texture::fromFile((snakeTexturePath() + "body_d2r_dig_128.png").c_str())},

	TAIL_D2U_F1{Texture::fromFile((snakeTexturePath() + "tail_d2u_f1_128.png").c_str())},
	TAIL_D2U_F2{Texture::fromFile((snakeTexturePath() + "tail_d2u_f2_128.png").c_str())},
	TAIL_D2U_DIG_F1{Texture::fromFile((snakeTexturePath() + "tail_d2u_dig_f1_128.png").c_str())},
	TAIL_D2U_DIG_F2{Texture::fromFile((snakeTexturePath() + "tail_d2u_dig_f2_128.png").c_str())},
	TAIL_D2R_F1{Texture::fromFile((snakeTexturePath() + "tail_d2r_f1_128.png").c_str())},
	TAIL_D2R_F2{Texture::fromFile((snakeTexturePath() + "tail_d2r_f2_128.png").c_str())},
	TAIL_D2R_DIG_F1{Texture::fromFile((snakeTexturePath() + "tail_d2r_dig_f1_128.png").c_str())},
	TAIL_D2R_DIG_F2{Texture::fromFile((snakeTexturePath() + "tail_d2r_dig_f2_128.png").c_str())},

	BUTTON_LEFT{Texture::fromFile((snakeTexturePath() + "button_left_128.png").c_str())},
	BUTTON_LEFT_TOUCHED{Texture::fromFile((snakeTexturePath() + "button_left_touched_128.png").c_str())},
	BUTTON_LEFT_DISABLED{Texture::fromFile((snakeTexturePath() + "button_left_disabled_128.png").c_str())},
	BUTTON_MIDDLE_TOUCHED{Texture::fromFile((snakeTexturePath() + "button_middle_touched_128.png").c_str())},
	BUTTON_RIGHT{Texture::fromFile((snakeTexturePath() + "button_right_128.png").c_str())},
	BUTTON_RIGHT_TOUCHED{Texture::fromFile((snakeTexturePath() + "button_right_touched_128.png").c_str())},
	BUTTON_RIGHT_DISABLED{Texture::fromFile((snakeTexturePath() + "button_right_disabled_128.png").c_str())},

	OBJECT{Texture::fromFile((snakeTexturePath() + "object_128.png").c_str())},
	BONUS_OBJECT{Texture::fromFile((snakeTexturePath() + "bonus_object_128.png").c_str())},
	FILLED{Texture::fromFile((snakeTexturePath() + "filled_64.png").c_str())},
	TILE_FACE{Texture::fromFile((snakeTexturePath() + "tile_face_128.png").c_str())},

	ATLAS_128{snakeTexturePath(), {
		"head_d2u_f1_128.png",
		"head_d2u_f2_128.png",
		"pre_head_d2u_f1_128.png",
		"pre_head_d2u_dig_f1_128.png",
		"pre_head_d2r_f1_128.png",
		"pre_head_d2r_dig_f1_128.png",

		"dead_pre_head_d2u_f1_128.png",
		"dead_pre_head_d2u_dig_f1_128.png",
		"dead_pre_head_d2r_f1_128.png",
		"dead_pre_head_d2r_dig_f1_128.png",

		"body_d2u_128.png",
		"body_d2u_dig_128.png",
		"body_d2r_128.png",
		"body_d2r_dig_128.png",

		"tail_d2u_f1_128.png",
		"tail_d2u_f2_128.png",
		"tail_d2u_dig_f1_128.png",
		"tail_d2u_dig_f2_128.png",
		"tail_d2r_f1_128.png",
		"tail_d2r_f2_128.png",
		"tail_d2r_dig_f1_128.png",
		"tail_d2r_dig_f2_128.png",

		"button_left_128.png",
		"button_left_touched_128.png",
		"button_left_disabled_128.png",
		"button_middle_touched_128.png",
		"button_right_128.png",
		"button_right_touched_128.png",
		"button_right_disabled_128.png",

		"object_128.png",
		"bonus_object_128.png",
		"filled_64.png",
		"tile_face_128.png"

	}},
	HEAD_D2U_F1_REG{*ATLAS_128.textureRegion("head_d2u_f1_128.png")},
	HEAD_D2U_F2_REG{*ATLAS_128.textureRegion("head_d2u_f2_128.png")},
		
	PRE_HEAD_D2U_F1_REG{*ATLAS_128.textureRegion("pre_head_d2u_f1_128.png")},
	PRE_HEAD_D2U_DIG_F1_REG{*ATLAS_128.textureRegion("pre_head_d2u_dig_f1_128.png")},
	PRE_HEAD_D2R_F1_REG{*ATLAS_128.textureRegion("pre_head_d2r_f1_128.png")},
	PRE_HEAD_D2R_DIG_F1_REG{*ATLAS_128.textureRegion("pre_head_d2r_dig_f1_128.png")},

	DEAD_PRE_HEAD_D2U_F1_REG{*ATLAS_128.textureRegion("head_d2u_f1_128.png")},
	DEAD_PRE_HEAD_D2U_DIG_F1_REG{*ATLAS_128.textureRegion("head_d2u_f1_128.png")},
	DEAD_PRE_HEAD_D2R_F1_REG{*ATLAS_128.textureRegion("head_d2u_f1_128.png")},
	DEAD_PRE_HEAD_D2R_DIG_F1_REG{*ATLAS_128.textureRegion("head_d2u_f1_128.png")},

	BODY_D2U_REG{*ATLAS_128.textureRegion("body_d2u_128.png")},
	BODY_D2U_DIG_REG{*ATLAS_128.textureRegion("body_d2u_dig_128.png")},
	BODY_D2R_REG{*ATLAS_128.textureRegion("body_d2r_128.png")},
	BODY_D2R_DIG_REG{*ATLAS_128.textureRegion("body_d2r_dig_128.png")},

	TAIL_D2U_F1_REG{*ATLAS_128.textureRegion("tail_d2u_f1_128.png")},
	TAIL_D2U_F2_REG{*ATLAS_128.textureRegion("tail_d2u_f2_128.png")},
	TAIL_D2U_DIG_F1_REG{*ATLAS_128.textureRegion("tail_d2u_dig_f1_128.png")},
	TAIL_D2U_DIG_F2_REG{*ATLAS_128.textureRegion("tail_d2u_dig_f2_128.png")},
	TAIL_D2R_F1_REG{*ATLAS_128.textureRegion("tail_d2r_f1_128.png")},
	TAIL_D2R_F2_REG{*ATLAS_128.textureRegion("tail_d2r_f2_128.png")},
	TAIL_D2R_DIG_F1_REG{*ATLAS_128.textureRegion("tail_d2r_dig_f1_128.png")},
	TAIL_D2R_DIG_F2_REG{*ATLAS_128.textureRegion("tail_d2r_dig_f2_128.png")},

	BUTTON_LEFT_REG{*ATLAS_128.textureRegion("button_left_128.png")},
	BUTTON_LEFT_TOUCHED_REG{*ATLAS_128.textureRegion("button_left_touched_128.png")},
	BUTTON_LEFT_DISABLED_REG{*ATLAS_128.textureRegion("button_left_disabled_128.png")},
	BUTTON_MIDDLE_TOUCHED_REG{*ATLAS_128.textureRegion("button_middle_touched_128.png")},
	BUTTON_RIGHT_REG{*ATLAS_128.textureRegion("button_right_128.png")},
	BUTTON_RIGHT_TOUCHED_REG{*ATLAS_128.textureRegion("button_right_touched_128.png")},
	BUTTON_RIGHT_DISABLED_REG{*ATLAS_128.textureRegion("button_right_disabled_128.png")},

	OBJECT_REG{*ATLAS_128.textureRegion("object_128.png")},
	BONUS_OBJECT_REG{*ATLAS_128.textureRegion("bonus_object_128.png")},
	FILLED_REG{*ATLAS_128.textureRegion("filled_64.png")},
	TILE_FACE_REG{*ATLAS_128.textureRegion("tile_face_128.png")},

	ATLAS_1024{assetsPath() + "1024pix/", {
		"coffer_logo_1024x256.png",
		"skipifzero_logo_1024x256.png",
		"skipifzero_snakium_logo_1024x256.png",
		"snakium_ascii_logo_1024x256.png"
	}},
	SNAKIUM_LOGO_REG{*ATLAS_1024.textureRegion("snakium_ascii_logo_1024x256.png")},
	SKIPIFZERO_LOGO_REG{*ATLAS_1024.textureRegion("skipifzero_logo_1024x256.png")},
	SKIPIFZERO_LOGO_SNAKIUM_VER_REG{*ATLAS_1024.textureRegion("skipifzero_snakium_logo_1024x256.png")},
	COFFER_LOGO_REG{*ATLAS_1024.textureRegion("coffer_logo_1024x256.png")},

	HEAD_D2U_F1_MODEL{modelPath(), "head_d2u_f1.obj", true, false},
	HEAD_D2U_F1_PROJECTION_MODEL{modelPath(), "head_d2u_f1_projection.obj", true, false},
	HEAD_D2U_F2_MODEL{modelPath(), "head_d2u_f2.obj", true, false},
	HEAD_D2U_F2_PROJECTION_MODEL{modelPath(), "head_d2u_f2_projection.obj", true, false},
	HEAD_ASC_F1_MODEL{modelPath(), "head_asc_f1.obj", true, false},
	HEAD_ASC_F1_PROJECTION_MODEL{modelPath(), "head_asc_f1_projection.obj", true, false},
	HEAD_ASC_F2_MODEL{modelPath(), "head_asc_f2.obj", true, false},
	HEAD_ASC_F2_PROJECTION_MODEL{modelPath(), "head_asc_f2_projection.obj", true, false},

	PRE_HEAD_D2U_F1_MODEL{modelPath(), "pre_head_d2u_f1.obj", true, false},
	PRE_HEAD_D2U_F1_PROJECTION_MODEL{modelPath(), "pre_head_d2u_f1_projection.obj", true, false},
	PRE_HEAD_D2U_DIG_F1_MODEL{modelPath(), "pre_head_d2u_dig_f1.obj", true, false},
	PRE_HEAD_D2U_DIG_F1_PROJECTION_MODEL{modelPath(), "pre_head_d2u_dig_f1_projection.obj", true, false},
	PRE_HEAD_D2R_F1_MODEL{modelPath(), "pre_head_d2r_f1.obj", true, false},
	PRE_HEAD_D2R_F1_PROJECTION_MODEL{modelPath(), "pre_head_d2r_f1_projection.obj", true, false},
	PRE_HEAD_D2R_DIG_F1_MODEL{modelPath(), "pre_head_d2r_dig_f1.obj", true, false},
	PRE_HEAD_D2R_DIG_F1_PROJECTION_MODEL{modelPath(), "pre_head_d2r_dig_f1_projection.obj", true, false},
	PRE_HEAD_D2L_F1_MODEL{modelPath(), "pre_head_d2l_f1.obj", true, false},
	PRE_HEAD_D2L_F1_PROJECTION_MODEL{modelPath(), "pre_head_d2l_f1_projection.obj", true, false},
	PRE_HEAD_D2L_DIG_F1_MODEL{modelPath(), "pre_head_d2l_dig_f1.obj", true, false},
	PRE_HEAD_D2L_DIG_F1_PROJECTION_MODEL{modelPath(), "pre_head_d2l_dig_f1_projection.obj", true, false},
	PRE_HEAD_DIVE_F1_MODEL{modelPath(), "pre_head_dive_f1.obj", true, false},
	PRE_HEAD_DIVE_F1_PROJECTION_MODEL{modelPath(), "pre_head_dive_f1_projection.obj", true, false},

	DEAD_PRE_HEAD_D2U_F1_MODEL{modelPath(), "dead_pre_head_d2u_f1.obj", true, false},
	DEAD_PRE_HEAD_D2U_DIG_F1_MODEL{modelPath(), "dead_pre_head_d2u_dig_f1.obj", true, false},
	DEAD_PRE_HEAD_D2R_F1_MODEL{modelPath(), "dead_pre_head_d2r_f1.obj", true, false},
	DEAD_PRE_HEAD_D2R_DIG_F1_MODEL{modelPath(), "dead_pre_head_d2r_dig_f1.obj", true, false},
	DEAD_PRE_HEAD_D2L_F1_MODEL{modelPath(), "dead_pre_head_d2l_f1.obj", true, false},
	DEAD_PRE_HEAD_D2L_DIG_F1_MODEL{modelPath(), "dead_pre_head_d2l_dig_f1.obj", true, false},

	BODY_D2U_MODEL{modelPath(), "body_d2u.obj", true, false},
	BODY_D2U_PROJECTION_MODEL{modelPath(), "body_d2u_projection.obj", true, false},
	BODY_D2U_DIG_MODEL{modelPath(), "body_d2u_dig.obj", true, false},
	BODY_D2U_DIG_PROJECTION_MODEL{modelPath(), "body_d2u_dig_projection.obj", true, false},
	BODY_D2R_MODEL{modelPath(), "body_d2r.obj", true, false},
	BODY_D2R_PROJECTION_MODEL{modelPath(), "body_d2r_projection.obj", true, false},
	BODY_D2R_DIG_MODEL{modelPath(), "body_d2r_dig.obj", true, false},
	BODY_D2R_DIG_PROJECTION_MODEL{modelPath(), "body_d2r_dig_projection.obj", true, false},
	BODY_D2L_MODEL{modelPath(), "body_d2l.obj", true, false},
	BODY_D2L_PROJECTION_MODEL{modelPath(), "body_d2l_projection.obj", true, false},
	BODY_D2L_DIG_MODEL{modelPath(), "body_d2l_dig.obj", true, false},
	BODY_D2L_DIG_PROJECTION_MODEL{modelPath(), "body_d2l_dig_projection.obj", true, false},

	TAIL_D2U_F1_MODEL{modelPath(), "tail_d2u_f1.obj", true, false},
	TAIL_D2U_F1_PROJECTION_MODEL{modelPath(), "tail_d2u_f1_projection.obj", true, false},
	TAIL_D2U_DIG_F1_MODEL{modelPath(), "tail_d2u_dig_f1.obj", true, false},
	TAIL_D2U_DIG_F1_PROJECTION_MODEL{modelPath(), "tail_d2u_dig_f1_projection.obj", true, false},
	TAIL_D2U_F2_MODEL{modelPath(), "tail_d2u_f2.obj", true, false},
	TAIL_D2U_F2_PROJECTION_MODEL{modelPath(), "tail_d2u_f2_projection.obj", true, false},
	TAIL_D2U_DIG_F2_MODEL{modelPath(), "tail_d2u_dig_f2.obj", true, false},
	TAIL_D2U_DIG_F2_PROJECTION_MODEL{modelPath(), "tail_d2u_dig_f2_projection.obj", true, false},
	TAIL_D2R_F1_MODEL{modelPath(), "tail_d2r_f1.obj", true, false},
	TAIL_D2R_F1_PROJECTION_MODEL{modelPath(), "tail_d2r_f1_projection.obj", true, false},
	TAIL_D2R_DIG_F1_MODEL{modelPath(), "tail_d2r_dig_f1.obj", true, false},
	TAIL_D2R_DIG_F1_PROJECTION_MODEL{modelPath(), "tail_d2r_dig_f1_projection.obj", true, false},
	TAIL_D2R_F2_MODEL{modelPath(), "tail_d2r_f2.obj", true, false},
	TAIL_D2R_F2_PROJECTION_MODEL{modelPath(), "tail_d2r_f2_projection.obj", true, false},
	TAIL_D2R_DIG_F2_MODEL{modelPath(), "tail_d2r_dig_f2.obj", true, false},
	TAIL_D2R_DIG_F2_PROJECTION_MODEL{modelPath(), "tail_d2r_dig_f2_projection.obj", true, false},
	TAIL_D2L_F1_MODEL{modelPath(), "tail_d2l_f1.obj", true, false},
	TAIL_D2L_F1_PROJECTION_MODEL{modelPath(), "tail_d2l_f1_projection.obj", true, false},
	TAIL_D2L_DIG_F1_MODEL{modelPath(), "tail_d2l_dig_f1.obj", true, false},
	TAIL_D2L_DIG_F1_PROJECTION_MODEL{modelPath(), "tail_d2l_dig_f1_projection.obj", true, false},
	TAIL_D2L_F2_MODEL{modelPath(), "tail_d2l_f2.obj", true, false},
	TAIL_D2L_F2_PROJECTION_MODEL{modelPath(), "tail_d2l_f2_projection.obj", true, false},
	TAIL_D2L_DIG_F2_MODEL{modelPath(), "tail_d2l_dig_f2.obj", true, false},
	TAIL_D2L_DIG_F2_PROJECTION_MODEL{modelPath(), "tail_d2l_dig_f2_projection.obj", true, false},

	OBJECT_MODEL{modelPath(), "object.obj", true, false},
	BONUS_OBJECT_MODEL{modelPath(), "bonus_object.obj", true, false},

	TILE_DECORATION_MODEL{modelPath(), "tile_decoration.obj", true, false},
	TILE_PROJECTION_MODEL{modelPath(), "tile_projection.obj", true, false},

	NOT_FOUND_MODEL{modelPath(), "notfound.obj", true, false}
{ }

} // namespace s3