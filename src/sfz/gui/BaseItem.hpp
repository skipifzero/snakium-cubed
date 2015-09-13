#pragma once
#ifndef SFZ_GUI_BASE_ITEM_HPP
#define SFZ_GUI_BASE_ITEM_HPP

#include <cstdint>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/math/Vector.hpp"
#include "sfz/sdl/ButtonState.hpp"
#include "sfz/gui/InputData.hpp"

namespace gui {

using sfz::AABB2D;
using sfz::vec2;

using std::uint32_t;

// BaseItem class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class BaseItem {
public:
	friend class System;

	// Virtual methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel) = 0;
	virtual KeyInput input(KeyInput key) = 0;
	virtual void update(float delta) = 0;
	virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam) = 0;
	
	// Virtual getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool isSelected() const = 0;
	virtual bool isEnabled() const = 0;

	// Virtual setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual void deselect() = 0;
	virtual void enable() = 0;
	virtual void disable() = 0;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	vec2 offset, dim;

	// Common inline functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline AABB2D bounds(vec2 basePos) const noexcept
	{
		return AABB2D{basePos + offset, dim};
	}

	inline AABB2D bounds(const AABB2D& rect) const noexcept
	{
		return bounds(rect.position());
	}
};

} // namespace gui
#endif