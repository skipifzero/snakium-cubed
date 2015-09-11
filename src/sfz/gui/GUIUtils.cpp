#include "sfz/gui/GUIUtils.hpp"

namespace gui {

AABB2D calculateGUICamera(vec2 drawableDim, vec2 minDrawable) noexcept
{
	const float minDrawableAspect = minDrawable.x / minDrawable.y;
	const float drawableAspect = drawableDim.x / drawableDim.y;

	vec2 camDim;
	if (minDrawableAspect > drawableAspect) {
		camDim = vec2{minDrawable.x, minDrawable.x*(drawableDim.y/drawableDim.x)};
	} else {
		camDim = vec2{minDrawable.y*(drawableDim.x/drawableDim.y), minDrawable.y};
	}
	return AABB2D{minDrawable/2.0f, camDim};
}

} // namespace gui