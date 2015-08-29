#pragma once
#ifndef SFZ_GUI_SYSTEM_HPP
#define SFZ_GUI_SYSTEM_HPP

#include <memory>
#include <vector>

#include "sfz/geometry/Rectangle.hpp"
#include "sfz/gl/Alignment.hpp"
#include "sfz/math/Vector.hpp"
#include "sfz/sdl/ButtonState.hpp"

#include "sfz/gui/BaseItem.hpp"
#include "sfz/gui/InputData.hpp"

namespace gui {

using gl::HorizontalAlign;

using sfz::Rectangle;
using sfz::vec2;

using std::shared_ptr;
using std::vector;

// System class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class System final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	System() = delete;
	System(const System&) = delete;
	System& operator= (const System) = delete;

	System(const Rectangle& bounds);

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool addItem(shared_ptr<BaseItem> item, vec2 dim,
	             HorizontalAlign hAlign = HorizontalAlign::CENTER) noexcept;
	bool addSpacing(float amount) noexcept;

	void update(InputData data);
	void draw(vec2 drawableDim, vec2 camPos, vec2 camDim);

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	inline const Rectangle& bounds() const noexcept { return mBounds; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Rectangle mBounds;
	vector<shared_ptr<BaseItem>> mItems;
	vec2 mNextItemTopPos;
	int mCurrentSelectedIndex = -1;
};

} // namespace gui
#endif