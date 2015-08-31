#include "sfz/gui/Button.hpp"

#include "sfz/geometry/Intersection2D.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// Button: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Button::Button(void(*activateFuncPtr)(Button&)) noexcept
:
	text{""},
	activateFuncPtr{activateFuncPtr}
{ }

Button::Button(const string& text, void(*activateFuncPtr)(Button&)) noexcept
:
	text{text},
	activateFuncPtr{activateFuncPtr}
{ }

// Button: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool Button::update(vec2 pointerPos, sdl::ButtonState pointerState, vec2)
{
	if (!mEnabled) return false;
	mSelected = sfz::pointInside(bounds, pointerPos);
	if (pointerState == sdl::ButtonState::UP && activateFuncPtr != nullptr) {
		activateFuncPtr(*this);
	}
	return mSelected;
}

KeyInput Button::update(KeyInput key)
{
	if (!mEnabled) return key;
	if (mSelected) {
		if (key == KeyInput::ACTIVATE) {
			if (activateFuncPtr != nullptr) activateFuncPtr(*this);
			return KeyInput::NONE;
		}
		else if (key == KeyInput::DOWN || key == KeyInput::UP) {
			mSelected = false;
			return key;
		}
		return KeyInput::NONE;
	} else {
		if (key == KeyInput::DOWN || key == KeyInput::UP) {
			mSelected = true;
		}
		return KeyInput::NONE;
	}
}

void Button::draw(unsigned int fbo, vec2 drawableDim, vec2 camPos, vec2 camDim)
{
	auto& assets = s3::Assets::INSTANCE();
	auto& sb = assets.spriteBatch;
	auto& font = assets.fontRenderer;

	/*sb.begin(camPos, camDim);
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
	font.end(0, drawableDim, mSelected ? sfz::vec4{1.0f, 0.0f, 0.0f, 1.0f} : sfz::vec4{1.0f});*/

	using sfz::vec4;
	auto& b = *this;

	// Select the correct texture regions and font colors
	sfz::TextureRegion* leftRegion = &assets.BUTTON_LEFT_REG;
	sfz::TextureRegion* midRegion = nullptr;
	sfz::TextureRegion* rightRegion = &assets.BUTTON_RIGHT_REG;
	vec4 fgColor{0.84f, 1.0f, 0.84f, 1.0f};
	vec4 bgColor{0.37f, 0.72f, 0.37f, 1.0f};

	if (!b.isEnabled()) {
		leftRegion = &assets.BUTTON_LEFT_DISABLED_REG;
		rightRegion = &assets.BUTTON_RIGHT_DISABLED_REG;
		fgColor = vec4{0.80f, 0.80f, 0.80f, 1.0f};
		bgColor = vec4{0.80f, 0.80f, 0.80f, 0.5f};
	}
	else if (b.isSelected()) {
		leftRegion = &assets.BUTTON_LEFT_TOUCHED_REG;
		midRegion = &assets.BUTTON_MIDDLE_TOUCHED_REG;
		rightRegion = &assets.BUTTON_RIGHT_TOUCHED_REG;
		fgColor = vec4{0.0f, 0.0f, 0.0f, 1.0f};
		bgColor = vec4{0.0f, 0.0f, 0.0f, 0.5f};
	}

	// Render the button
	sb.begin(camPos, camDim);
	const auto& r = b.bounds;
	sb.draw(r.pos - vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *leftRegion);
	if (midRegion != nullptr) {
		sb.draw(r.pos, vec2{r.dim.x - 0.975f*r.dim.y, r.dim.y}, *midRegion);
	}
	sb.draw(r.pos + vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *rightRegion);
	sb.end(fbo, drawableDim, assets.ATLAS_128.texture());

	// Font rendering preparations
	font.horizontalAlign(gl::HorizontalAlign::CENTER);
	font.verticalAlign(gl::VerticalAlign::BOTTOM);
	const float size = 0.6f * r.dim.y;
	const float yAlignOffset = (r.dim.y/2.0f)*0.3f;
	const float bgXAlignOffset = size * 0.035f;

	// Render button text
	// Font shadow
	font.begin(camPos, camDim);
	font.write(vec2{r.pos.x + bgXAlignOffset, r.pos.y - yAlignOffset}, size, b.text);
	font.end(fbo, drawableDim, bgColor);
	// Font foreground
	font.begin(camPos, camDim);
	font.write(vec2{r.pos.x, r.pos.y - yAlignOffset}, size, b.text);
	font.end(fbo, drawableDim, fgColor);
}

void Button::move(vec2 diff)
{
	bounds.pos += diff;
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