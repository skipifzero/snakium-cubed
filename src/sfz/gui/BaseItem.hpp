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

	virtual void updatePointer(vec2 pointerPos, sdl::ButtonState pointerState) = 0;
	virtual void updateKey(KeyInput key) = 0;
	virtual void draw(vec2 drawableDim, vec2 camPos, vec2 camDim) = 0;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const Rectangle& bounds() const noexcept { return mBounds; }

protected:
	// Protected members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	Rectangle mBounds;
};	

} // namespace gui
#endif