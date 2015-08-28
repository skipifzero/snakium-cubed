#include "sfz/gui/Button.hpp"

#include "sfz/geometry/Intersection2D.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// Button: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Button::Button(void(*activateFuncPtr)(Button&)) noexcept
:
	mText{""},
	mActivateFuncPtr{activateFuncPtr}
{ }

Button::Button(const string& text, void(*activateFuncPtr)(Button&)) noexcept
:
	mText{text},
	mActivateFuncPtr{activateFuncPtr}
{ }

// Button: Public methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Button::updatePointer(vec2 pointerPos, sdl::ButtonState pointerState)
{

}

void Button::updateKey(KeyInput key)
{

}

void Button::draw(vec2 drawableDim, vec2 camPos, vec2 camDim)
{
	auto& assets = s3::Assets::INSTANCE();
	auto& sb = assets.spriteBatch;
	auto& font = assets.fontRenderer;

	sb.begin(camPos, camDim);
	sb.draw(mBounds.pos, mBounds.dim, assets.TILE_FACE_REG);
	sb.end(0, drawableDim, assets.ATLAS_128.texture());

	// Font rendering preparations
	font.horizontalAlign(gl::HorizontalAlign::CENTER);
	font.verticalAlign(gl::VerticalAlign::BOTTOM);
	const float size = 0.6f * mBounds.dim.y;
	const float yAlignOffset = (mBounds.dim.y/2.0f)*0.3f;
	const float bgXAlignOffset = mBounds.dim.x * 0.006f;

	// Render button text
	font.begin(camPos, camDim);
	font.write(vec2{mBounds.pos.x, mBounds.pos.y - yAlignOffset}, size, mText);
	font.end(0, drawableDim, sfz::vec4{1.0f});
}

/*// Button: Constructors & destructors
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
}*/

} // namespace gui