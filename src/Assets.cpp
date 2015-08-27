#include "Assets.hpp"

namespace s3 {

namespace {

const std::string& basePath() noexcept
{
	static const std::string BASE_PATH{SDL_GetBasePath()};
	return BASE_PATH;
}

const std::string& assetsPath() noexcept
{
	static const std::string ASSETS_PATH{basePath() + "assets/"};
	return ASSETS_PATH;
}

const std::string& snakeTexturePath() noexcept
{
	static const std::string SNAKE_TEXTURE_PATH{assetsPath() + "128pix/"};
	return SNAKE_TEXTURE_PATH;
}

} // anonymous namespace

// Assets: Singleton instance
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Assets& Assets::INSTANCE() noexcept
{
	static Assets instance;
	return instance;
}

// Assets: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const gl::Texture& Assets::getTileTexture(SnakeTile *tilePtr, float progress, bool gameOver) const noexcept
{
	bool isTurn = s3::isTurn(tilePtr->from(), tilePtr->to());

	switch (tilePtr->type()) {
	case s3::TileType::EMPTY: return TILE_FACE;
	case s3::TileType::OBJECT: return OBJECT;
	case s3::TileType::BONUS_OBJECT: return BONUS_OBJECT;

	case s3::TileType::HEAD:
		if (progress <= 0.5f) { // Frame 1
			return HEAD_D2U_F1;
		}
		else { // Frame 2
			return HEAD_D2U_F2;
		}
	case s3::TileType::PRE_HEAD:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return !gameOver ? PRE_HEAD_D2U_F1 : DEAD_PRE_HEAD_D2U_F1;
			else return !gameOver ? PRE_HEAD_D2R_F1 : DEAD_PRE_HEAD_D2R_F1;
		}
		else { // Frame 2
			if (!isTurn) return BODY_D2U;
			else return BODY_D2R;
		}
	case s3::TileType::BODY:
		if (!isTurn) return BODY_D2U;
		else return BODY_D2R;
	case s3::TileType::TAIL:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return TAIL_D2U_F1;
			else return TAIL_D2R_F1;
		}
		else { // Frame 2
			if (!isTurn) return TAIL_D2U_F2;
			else return TAIL_D2R_F2;
		}

	case s3::TileType::HEAD_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			return HEAD_D2U_F1;
		}
		else { // Frame 2
			return HEAD_D2U_F2;
		}
	case s3::TileType::PRE_HEAD_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return !gameOver ? PRE_HEAD_D2U_DIG_F1 : DEAD_PRE_HEAD_D2U_DIG_F1;
			else return !gameOver ? PRE_HEAD_D2R_DIG_F1 : DEAD_PRE_HEAD_D2R_DIG_F1;
		}
		else { // Frame 2
			if (!isTurn) return BODY_D2U_DIG;
			else return BODY_D2R_DIG;
		}
	case s3::TileType::BODY_DIGESTING:
		if (!isTurn) return BODY_D2U_DIG;
		else return BODY_D2R_DIG;
	case s3::TileType::TAIL_DIGESTING:
		if (progress <= 0.5f) { // Frame 1
			if (!isTurn) return TAIL_D2U_DIG_F1;
			else return TAIL_D2R_DIG_F1;
		}
		else { // Frame 2
			if (!isTurn) return TAIL_D2U_DIG_F2;
			else return TAIL_D2R_DIG_F2;
		}
	}
}

// Assets: Private constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Assets::Assets() noexcept
:
	spriteBatch{3000},
	fontRenderer{assetsPath() + "fonts/SourceCodePro-Regular.ttf", 1024, 1024, 74.0f, 3000},

	HEAD_D2U_F1{snakeTexturePath() + "head_d2u_f1_128.png"},
	HEAD_D2U_F2{snakeTexturePath() + "head_d2u_f2_128.png"},

	PRE_HEAD_D2U_F1{snakeTexturePath() + "pre_head_d2u_f1_128.png"},
	PRE_HEAD_D2U_DIG_F1{snakeTexturePath() + "pre_head_d2u_dig_f1_128.png"},
	PRE_HEAD_D2R_F1{snakeTexturePath() + "pre_head_d2r_f1_128.png"},
	PRE_HEAD_D2R_DIG_F1{snakeTexturePath() + "pre_head_d2r_dig_f1_128.png"},

	DEAD_PRE_HEAD_D2U_F1{snakeTexturePath() + "dead_pre_head_d2u_f1_128.png"},
	DEAD_PRE_HEAD_D2U_DIG_F1{snakeTexturePath() + "dead_pre_head_d2u_dig_f1_128.png"},
	DEAD_PRE_HEAD_D2R_F1{snakeTexturePath() + "dead_pre_head_d2r_f1_128.png"},
	DEAD_PRE_HEAD_D2R_DIG_F1{snakeTexturePath() + "dead_pre_head_d2r_dig_f1_128.png"},

	BODY_D2U{snakeTexturePath() + "body_d2u_128.png"},
	BODY_D2U_DIG{snakeTexturePath() + "body_d2u_dig_128.png"},
	BODY_D2R{snakeTexturePath() + "body_d2r_128.png"},
	BODY_D2R_DIG{snakeTexturePath() + "body_d2r_dig_128.png"},

	TAIL_D2U_F1{snakeTexturePath() + "tail_d2u_f1_128.png"},
	TAIL_D2U_F2{snakeTexturePath() + "tail_d2u_f2_128.png"},
	TAIL_D2U_DIG_F1{snakeTexturePath() + "tail_d2u_dig_f1_128.png"},
	TAIL_D2U_DIG_F2{snakeTexturePath() + "tail_d2u_dig_f2_128.png"},
	TAIL_D2R_F1{snakeTexturePath() + "tail_d2r_f1_128.png"},
	TAIL_D2R_F2{snakeTexturePath() + "tail_d2r_f2_128.png"},
	TAIL_D2R_DIG_F1{snakeTexturePath() + "tail_d2r_dig_f1_128.png"},
	TAIL_D2R_DIG_F2{snakeTexturePath() + "tail_d2r_dig_f2_128.png"},

	BUTTON_LEFT{snakeTexturePath() + "button_left_128.png"},
	BUTTON_LEFT_TOUCHED{snakeTexturePath() + "button_left_touched_128.png"},
	BUTTON_LEFT_DISABLED{snakeTexturePath() + "button_left_disabled_128.png"},
	BUTTON_MIDDLE_TOUCHED{snakeTexturePath() + "button_middle_touched_128.png"},
	BUTTON_RIGHT{snakeTexturePath() + "button_right_128.png"},
	BUTTON_RIGHT_TOUCHED{snakeTexturePath() + "button_right_touched_128.png"},
	BUTTON_RIGHT_DISABLED{snakeTexturePath() + "button_right_disabled_128.png"},

	OBJECT{snakeTexturePath() + "object_128.png"},
	BONUS_OBJECT{snakeTexturePath() + "bonus_object_128.png"},
	FILLED{snakeTexturePath() + "filled_64.png"},
	TILE_FACE{snakeTexturePath() + "tile_face_128.png"},

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
	COFFER_LOGO_REG{*ATLAS_1024.textureRegion("coffer_logo_1024x256.png")}
{ }

} // namespace s3