#pragma once
#ifndef SFZ_MATH_MATHCONSTANTS_HPP
#define SFZ_MATH_MATHCONSTANTS_HPP

#include "sfz/SFZCommonGlobal.hpp"

#include <cmath>

namespace sfz {

const float g_PI_FLOAT{std::atan(1.0f)*4.0f};
const double g_PI_DOUBLE{std::atan(1.0)*4.0};

const double g_DEG_TO_RAD_DOUBLE = g_PI_DOUBLE / 180.0;
const double g_RAD_TO_DEG_DOUBLE = 180.0 / g_PI_DOUBLE;

const float g_DEG_TO_RAD_FLOAT = g_PI_FLOAT / 180.0f;
const float g_RAD_TO_DEG_FLOAT = 180.0f / g_PI_FLOAT;

} // namespace sfz
#endif