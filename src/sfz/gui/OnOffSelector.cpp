#include "sfz/gui/OnOffSelector.hpp"

#include "sfz/geometry/Intersection.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// OnOffSelector: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OnOffSelector::OnOffSelector(const string& text, const function<bool(void)>& checkStateFunc,
                             const function<void(void)>& changeStateFunc, float stateAlignOffset)
:
	text{text + ":"},
	checkStateFunc{checkStateFunc},
	changeStateFunc{changeStateFunc},
	stateAlignOffset{stateAlignOffset}
{ }

// OnOffSelector: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool OnOffSelector::update(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	if (!mEnabled) return false;
	mSelected = sfz::pointInside(bounds(basePos), pointerPos);
	if (pointerState == sdl::ButtonState::UP) {
		if (changeStateFunc) changeStateFunc();
	}
	return mSelected;
}

KeyInput OnOffSelector::update(KeyInput key)
{
	if (!mEnabled) return key;
	if (mSelected) {
		if (key == KeyInput::ACTIVATE) {
			if (changeStateFunc) changeStateFunc();
			return KeyInput::NONE;
		}
		else if (key == KeyInput::DOWN || key == KeyInput::UP) {
			mSelected = false;
			return key;
		} else if (key == KeyInput::LEFT) {
			bool state = false;
			if (checkStateFunc) state = checkStateFunc();
			if (!state && changeStateFunc) changeStateFunc();
		} else if (key == KeyInput::RIGHT) {
			bool state = false;
			if (checkStateFunc) state = checkStateFunc();
			if (state && changeStateFunc) changeStateFunc();
		}
		return KeyInput::NONE;
	} else {
		if (key == KeyInput::DOWN || key == KeyInput::UP) {
			mSelected = true;
		}
		return KeyInput::NONE;
	}
}

void OnOffSelector::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	using sfz::vec4;

	auto& assets = s3::Assets::INSTANCE();
	auto& sb = assets.spriteBatch;
	auto& font = assets.fontRenderer;

	sb.begin(cam);
	sb.draw(basePos + offset, dim, assets.TILE_FACE_REG);
	sb.end(fbo, viewport, assets.ATLAS_128.texture());

	// Font rendering preparations
	font.horizontalAlign(gl::HorizontalAlign::LEFT);
	font.verticalAlign(gl::VerticalAlign::MIDDLE);
	const float size = 0.6f * dim.y;
	const float yAlignOffset = (dim.y/2.0f)*0.3f;
	const float bgXAlignOffset = dim.x * 0.009f;

	vec2 leftMiddlePos = basePos + offset + vec2{-(dim.x/2.0f), yAlignOffset};
	vec2 onPos = leftMiddlePos;
	onPos.x += std::max(stateAlignOffset, font.measureStringWidth(size, text + " "));
	vec2 offPos = onPos;
	offPos.x += font.measureStringWidth(size, "On ");

	// Render font shadow
	font.begin(cam);
	font.write(leftMiddlePos + vec2{bgXAlignOffset, 0.0f}, size, text);
	font.write(onPos + vec2{bgXAlignOffset, 0.0f}, size, "On");
	font.write(offPos + vec2{bgXAlignOffset, 0.0f}, size, "Off");
	font.end(fbo, viewport, vec4{0.0f, 0.0f, 0.0f, 1.0f});

	bool state = false;
	if (checkStateFunc) state = checkStateFunc();

	// Render button text
	font.begin(cam);
	font.write(leftMiddlePos, size, text);
	font.end(fbo, viewport, mSelected ? vec4{1.0f, 0.0f, 0.0f, 1.0f} : vec4{1.0f});
	font.begin(cam);
	font.write(onPos, size, "On");
	font.end(fbo, viewport, state ? (mSelected ? vec4{1.0f, 0.0f, 0.0f, 1.0f} : vec4{1.0f}) : vec4{0.5f, 0.5f, 0.5f, 1.0f});
	font.begin(cam);
	font.write(offPos, size, "Off");
	font.end(fbo, viewport, !state ? (mSelected ? vec4{1.0f, 0.0f, 0.0f, 1.0f} : vec4{1.0f}) : vec4{0.5f, 0.5f, 0.5f, 1.0f});
}

// OnOffSelector: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool OnOffSelector::isSelected() const
{
	return mSelected;
}

bool OnOffSelector::isEnabled() const
{
	return mEnabled;
}

// OnOffSelector: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void OnOffSelector::deselect()
{
	mSelected = false;
}

void OnOffSelector::enable()
{
	mEnabled = true;
}

void OnOffSelector::disable()
{
	mEnabled = false;
	mSelected = false;
}

} // namespace gui