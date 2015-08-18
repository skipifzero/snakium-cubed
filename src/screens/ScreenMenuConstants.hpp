#pragma once
#ifndef S3_SCREENS_SCREEN_MENU_CONSTANTS_HPP
#define S3_SCREENS_SCREEN_MENU_CONSTANTS_HPP

#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::vec2;

namespace screens {

const vec2 MIN_DRAWABLE{100.0f, 120.0f};

inline vec2 guiDimensions(vec2 drawableDimensions) noexcept
{
	const float minDrawableAspect = MIN_DRAWABLE.x / MIN_DRAWABLE.y;
	const float drawableAspect = drawableDimensions.x / drawableDimensions.y;

	if (minDrawableAspect > drawableAspect) {
		return vec2{MIN_DRAWABLE.x, MIN_DRAWABLE.x*(drawableDimensions.y/drawableDimensions.x)};
	} else {
		return vec2{MIN_DRAWABLE.y*(drawableDimensions.x/drawableDimensions.y), MIN_DRAWABLE.y};
	}
}

inline vec2 guiOffset(vec2 guiDimensions) noexcept
{
	return (MIN_DRAWABLE - guiDimensions)/2.0f;
}

} // namespace screens

} // namespace s3
#endif