#pragma once
#ifndef SFZ_GUI_SYSTEM_HPP
#define SFZ_GUI_SYSTEM_HPP

#include <memory>
#include <vector>

#include <sfz/geometry/Rectangle.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/sdl/ButtonState.hpp>

#include <sfz/gui/BaseItem.hpp>

namespace gui {

using sfz::Rectangle;
using sfz::vec2;

using std::shared_ptr;
using std::vector;

// InputData struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

enum class KeyInput {
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	ACTIVATE
	// TODO: CANCEL?
};

struct InputData {
	vec2 pointerPos; // Pointer pos in the same coordinate system as SystemRoot
	sdl::ButtonState pointerState;
	KeyInput key;
};

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

	void update(InputData data);
	void draw(vec2 drawableDim, vec2 guiDim, vec2 guiOffs);

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	inline const Rectangle& bounds() const noexcept { return mBounds; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Rectangle mBounds;
	vector<shared_ptr<BaseItem>> mItems;
	
	float mItamHeight, mPadding;
};

} // namespace gui
#endif