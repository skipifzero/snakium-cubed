#pragma once
#ifndef SFZ_GUI_BASE_ITEM_HPP
#define SFZ_GUI_BASE_ITEM_HPP

#include <sfz/geometry/Rectangle.hpp>
namespace gui { struct InputData; } // Forward declare InputData

namespace gui {

using sfz::Rectangle;

// BaseItem class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class BaseItem {
public:
	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual void update(const InputData& data) = 0;
	virtual void draw() = 0;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const Rectangle& bounds() const noexcept { return mBounds; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	Rectangle mBounds;
};

} // namespace gui
#endif