#pragma once
#ifndef SFZ_MATH_MATHCONSTANTS_HPP
#define SFZ_MATH_MATHCONSTANTS_HPP

#include <cmath>

namespace sfz {

template<typename T = float>
constexpr T PI() noexcept { return T(3.14159265358979323846); }

template<typename T = float>
constexpr T DEG_TO_RAD() noexcept { return PI<T>() / T(180); }

template<typename T = float>
constexpr T RAD_TO_DEG() noexcept { return T(180) / PI<T>(); }

} // namespace sfz
#endif