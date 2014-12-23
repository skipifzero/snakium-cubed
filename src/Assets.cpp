#include "Assets.hpp"

namespace s3 {

namespace {

const std::string& snakeTexturePath{"assets/128pix/"};

} // anonymous namespace

Assets::Assets() noexcept
:
	HEAD_D2U_F1{snakeTexturePath + "head_d2u_f1_128.png"},
	HEAD_D2U_F2{snakeTexturePath + "head_d2u_f2_128.png"},

	PRE_HEAD_D2U_F1{snakeTexturePath + "pre_head_d2u_f1_128.png"},
	PRE_HEAD_D2U_DIG_F1{snakeTexturePath + "pre_head_d2u_dig_f1_128.png"},
	PRE_HEAD_D2R_F1{snakeTexturePath + "pre_head_d2r_f1_128.png"},
	PRE_HEAD_D2R_DIG_F1{snakeTexturePath + "pre_head_d2r_dig_f1_128.png"},

	DEADHEAD_D2U_F3{snakeTexturePath + "deadhead_d2u_f3_128.png"},
	DEADHEAD_D2U_DIG_F3{snakeTexturePath + "deadhead_d2u_dig_f3_128.png"},
	DEADHEAD_D2R_F3{snakeTexturePath + "deadhead_d2r_f3_128.png"},
	DEADHEAD_D2R_DIG_F3{snakeTexturePath + "deadhead_d2r_dig_f3_128.png"},

	BODY_D2U{snakeTexturePath + "body_d2u_128.png"},
	BODY_D2U_DIG{snakeTexturePath + "body_d2u_dig_128.png"},
	BODY_D2R{snakeTexturePath + "body_d2r_128.png"},
	BODY_D2R_DIG{snakeTexturePath + "body_d2r_dig_128.png"},

	TAIL_D2U_F1{snakeTexturePath + "tail_d2u_f1_128.png"},
	TAIL_D2U_F2{snakeTexturePath + "tail_d2u_f2_128.png"},
	TAIL_D2U_DIG_F1{snakeTexturePath + "tail_d2u_dig_f1_128.png"},
	TAIL_D2U_DIG_F2{snakeTexturePath + "tail_d2u_dig_f2_128.png"},
	TAIL_D2R_F1{snakeTexturePath + "tail_d2r_f1_128.png"},
	TAIL_D2R_F2{snakeTexturePath + "tail_d2r_f2_128.png"},
	TAIL_D2R_DIG_F1{snakeTexturePath + "tail_d2r_dig_f1_128.png"},
	TAIL_D2R_DIG_F2{snakeTexturePath + "tail_d2r_dig_f2_128.png"},

	BUTTON_LEFT{snakeTexturePath + "button_left_128.png"},
	BUTTON_LEFT_TOUCHED{snakeTexturePath + "button_left_touched_128.png"},
	BUTTON_LEFT_DISABLED{snakeTexturePath + "button_left_disabled_128.png"},
	BUTTON_MIDDLE_TOUCHED{snakeTexturePath + "button_middle_touched_128.png"},
	BUTTON_RIGHT{snakeTexturePath + "button_right_128.png"},
	BUTTON_RIGHT_TOUCHED{snakeTexturePath + "button_right_touched_128.png"},
	BUTTON_RIGHT_DISABLED{snakeTexturePath + "button_right_disabled_128.png"},

	OBJECT{snakeTexturePath + "object_128.png"},
	BONUS_OBJECT{snakeTexturePath + "bonus_object_128.png"},
	FILLED{snakeTexturePath + "filled_64.png"},
	TILE_BORDER{snakeTexturePath + "tile_border_64.png"}
{

}

} // namespace s3