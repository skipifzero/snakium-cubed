#pragma once
#ifndef SFZ_GUI_BASE_ITEM_HPP
#define SFZ_GUI_BASE_ITEM_HPP

#include "sfz/geometry/Rectangle.hpp"
#include "sfz/math/Vector.hpp"
#include "sfz/sdl/ButtonState.hpp"
#include "sfz/gui/InputData.hpp"

namespace gui {

using sfz::Rectangle;
using sfz::vec2;

// BaseItem class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class BaseItem {
public:
	friend class System;

	// Virtual methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool update(vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel) = 0;
	virtual KeyInput update(KeyInput key) = 0;
	virtual void draw(unsigned int fbo, vec2 drawableDim, vec2 camPos, vec2 camDim) = 0;
	virtual void move(vec2 diff) = 0;
	
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
	
	Rectangle bounds;
};	

} // namespace gui
#endif