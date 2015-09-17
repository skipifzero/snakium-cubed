#include "sfz/gui/Button.hpp"

#include "sfz/geometry/Intersection.hpp"
#include "sfz/gui/RenderingSettings.hpp"

namespace gui {

// Button: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Button::Button(const function<void(Button&)>& activateFunc) noexcept
:
	text{""},
	activateFunc{activateFunc}
{ }

Button::Button(const string& text, const function<void(Button&)>& activateFunc)
:
	text{text},
	activateFunc{activateFunc}
{ }

// Button: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool Button::input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	if (!mEnabled) return false;
	mSelected = sfz::pointInside(bounds(basePos), pointerPos);
	if (pointerState == sdl::ButtonState::UP) {
		if (activateFunc) activateFunc(*this);
	}
	return mSelected;
}

KeyInput Button::input(KeyInput key)
{
	if (!mEnabled) return key;

	switch (key) {
	case KeyInput::UP:
	case KeyInput::DOWN:
	case KeyInput::LEFT:
	case KeyInput::RIGHT:
		if (mSelected) {
			mSelected = false;
			return key;
		} else {
			mSelected = true;
			return KeyInput::NONE; 
		}

	case KeyInput::ACTIVATE:
		if (mSelected && activateFunc) activateFunc(*this);
		return KeyInput::NONE;

	default:
	case KeyInput::NONE:
		return key;
	}
}

void Button::update(float delta)
{
	RenderingSettings::INSTANCE().buttonRenderer->update(*this, delta);
}

void Button::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	RenderingSettings::INSTANCE().buttonRenderer->draw(*this, basePos, fbo, viewport, cam);
}

// Button: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool Button::isSelected() const
{
	return mSelected;
}

bool Button::isEnabled() const
{
	return mEnabled;
}

// Button: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Button::deselect()
{
	mSelected = false;
}

void Button::enable()
{
	mEnabled = true;
}

void Button::disable()
{
	mEnabled = false;
	mSelected = false;
}

} // namespace gui