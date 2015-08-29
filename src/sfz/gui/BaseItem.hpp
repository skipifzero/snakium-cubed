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

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool update(vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel) = 0;
	virtual KeyInput update(KeyInput key) = 0;
	virtual void deselect() = 0;
	virtual void draw(vec2 drawableDim, vec2 camPos, vec2 camDim) = 0;
	virtual void move(vec2 diff) = 0;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const Rectangle& bounds() const noexcept { return mBounds; }

protected:
	// Protected members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	Rectangle mBounds;
	bool mSelected = false;
};	

} // namespace gui
#endif