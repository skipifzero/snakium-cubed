#pragma once
#ifndef S3_ASSETS_HPP
#define S3_ASSETS_HPP

#include "sfz/SDL.hpp"
#include "sfz/GL.hpp"
#include <string>

namespace s3 {

struct Assets final {
	gl::Texture HEAD_D2U_F1,
	            HEAD_D2U_F2,
				HEAD_D2U_F3,
				HEAD_D2U_DIG_F3,
				HEAD_D2R_F3,
				HEAD_D2R_DIG_F3,

				DEADHEAD_D2U_F3,
				DEADHEAD_D2U_DIG_F3,
				DEADHEAD_D2R_F3,
				DEADHEAD_D2R_DIG_F3,

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
				TAIL_D2R_DIG_F2;

	Assets() noexcept;
	~Assets() noexcept = default;
	Assets(const Assets&) = delete;
	Assets& operator= (const Assets&) = delete;
};

} // namespace s3

#endif