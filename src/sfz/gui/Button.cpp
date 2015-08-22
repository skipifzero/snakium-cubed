#include "sfz/gui/Button.hpp"

#include "sfz/geometry/Intersection2D.hpp"

namespace sfz {

// Button: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Button::Button(const Rectangle& rect, void(*funcPtr)(Button&)) noexcept
:
	mRect{rect},
	mText{""},
	mFuncPtr{mFuncPtr}
{ }

Button::Button(const Rectangle& rect, const string& text, void(*funcPtr)(Button&)) noexcept
:
	mRect{rect},
	mText{text},
	mFuncPtr{funcPtr}
{ }

// Button: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool Button::update(vec2 mousePos, bool clicked) noexcept
{
	// If button is disabled it is neither selected or activated
	if (!mEnabled) {
		mSelected = false;
		mActivated = false;
		return false;
	}

	// Checks if button is selected by mouse hoovering
	mSelected = pointInside(mRect, mousePos);

	// Checks if button is activated by clicking
	mActivated = mSelected && clicked;
	if (mActivated && mFuncPtr != nullptr) mFuncPtr(*this);

	return mActivated;
}

} // namespace sfz