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

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

AABB2D calculateViewport(const AABB2D& oldViewport, const AABB2D& camera,
                         const AABB2D& viewportArea) noexcept
{
	vec2 dim = (viewportArea.dimensions()/camera.dimensions())*oldViewport.dimensions();
	vec2 oldToNewCamPos = viewportArea.position() - camera.position();
	vec2 pos = oldViewport.position() + (oldToNewCamPos/camera.dimensions())*oldViewport.dimensions();
	return AABB2D{pos, dim};
}

} // namespace gui