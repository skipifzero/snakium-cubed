#pragma once
#ifndef S3_SCREENS_SCREEN_MENU_CONSTANTS_HPP
#define S3_SCREENS_SCREEN_MENU_CONSTANTS_HPP

#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;

// Menu dimensions
const vec2 MENU_SYSTEM_DIM = vec2{100.0f, 120.0f};
const vec2 MENU_DIM = MENU_SYSTEM_DIM - vec2{0.1f};

// Heading sizes
const float TITLE_HEIGHT = 20.0f;
const float NAVBAR_HEIGHT = 8.0f;

// Padding
const float MENU_TOP_PADDING = 2.0f;
const float MENU_BOTTOM_PADDING = 2.0f;

// Colors
const vec3 MENU_BG_COLOR = vec3{0.2f};

} // namespace s3
#endif