#include "sfz/gui/MultiChoiceSelector.hpp"

#include "sfz/geometry/Intersection.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// MultiChoiceSelector: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MultiChoiceSelector::MultiChoiceSelector(const string& text, const vector<string>& choiceNames,
                                         const function<int(void)>& checkStateFunc,
                                         const function<void(int)>& changeStateFunc,
                                         float stateAlignOffset)
:
	text{text + ":"},
	choiceNames{choiceNames},
	checkStateFunc{checkStateFunc},
	changeStateFunc{changeStateFunc},
	stateAlignOffset{stateAlignOffset}
{ }

// MultiChoiceSelector: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool MultiChoiceSelector::input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	if (!mEnabled) return false;
	mSelected = sfz::pointInside(bounds(basePos), pointerPos);
	if (pointerState == sdl::ButtonState::UP) {
		int currentState = checkStateFunc();
		int next = currentState + 1;
		if (currentState < 0) next = 0;
		if (next >= choiceNames.size()) next = 0;
		changeStateFunc(next);
	}
	return mSelected;
	return false;
}

KeyInput MultiChoiceSelector::input(KeyInput key)
{
	if (!mEnabled) return key;
	if (mSelected) {
		if (key == KeyInput::ACTIVATE) {
			int currentState = checkStateFunc();
			if (0 <= currentState && currentState <= choiceNames.size()-1) {
				int next = currentState + 1;
				if (next >= choiceNames.size()) next = 0;
				changeStateFunc(next);
			}
		}
		else if (key == KeyInput::DOWN || key == KeyInput::UP) {
			mSelected = false;
			return key;
		} else if (key == KeyInput::LEFT) {
			int currentState = checkStateFunc();
			int next = currentState - 1;
			if (currentState < 0) next = 0;
			if (next < 0) next = 0;
			if (next >= choiceNames.size()) next = choiceNames.size() - 1;
			if (next != currentState) changeStateFunc(next);
		} else if (key == KeyInput::RIGHT) {
			int currentState = checkStateFunc();
			int next = currentState + 1;
			if (currentState < 0) next = 0;
			if (next >= choiceNames.size()) next = choiceNames.size() - 1;
			if (next != currentState) changeStateFunc(next);
		}
		return KeyInput::NONE;
	} else {
		if (key == KeyInput::DOWN || key == KeyInput::UP) {
			mSelected = true;
		}
		return KeyInput::NONE;
	}
}

void MultiChoiceSelector::update(float delta)
{

}

void MultiChoiceSelector::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
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
	const float nearbySize = size*0.5f;
	const float yAlignOffset = (dim.y/2.0f)*0.3f;
	const float bgXAlignOffset = dim.x * 0.009f;

	const int choice = checkStateFunc();
	const int len = choiceNames.size();
	static const string empty = "";
	const string& choiceLeftStr = (0 <= (choice-1) && (choice-1) < len) ? choiceNames[choice-1] : empty;
	const string& choiceMidStr = (0 <= (choice) && (choice) < len) ? choiceNames[choice] : empty;
	const string& choiceRightStr = (0 <= (choice+1) && (choice+1) < len) ? choiceNames[choice+1] : empty;

	vec2 leftMiddlePos = basePos + offset + vec2{-(dim.x/2.0f), yAlignOffset};

	vec2 choiceLeftPos = leftMiddlePos;
	choiceLeftPos.x += std::max(stateAlignOffset, font.measureStringWidth(size, text + " "));
	vec2 choiceMidPos = choiceLeftPos;
	choiceMidPos.x += font.measureStringWidth(nearbySize, choiceLeftStr);
	vec2 choiceRightPos = choiceMidPos;
	choiceRightPos.x += font.measureStringWidth(size, choiceMidStr);

	// Render font shadow
	font.begin(cam);
	font.write(leftMiddlePos + vec2{bgXAlignOffset, 0.0f}, size, text);
	//font.write(choiceLeftPos + vec2{bgXAlignOffset, 0.0f}, nearbySize, choiceLeftStr);
	font.write(choiceMidPos + vec2{bgXAlignOffset, 0.0f}, size, choiceMidStr);
	//font.write(choiceRightPos + vec2{bgXAlignOffset, 0.0f}, nearbySize, choiceRightStr);
	font.end(fbo, viewport, vec4{0.0f, 0.0f, 0.0f, 1.0f});

	bool state = false;
	if (checkStateFunc) state = checkStateFunc();

	// Render button text
	font.begin(cam);
	font.write(leftMiddlePos, size, text);
	font.write(choiceMidPos, size, choiceMidStr);
	font.end(fbo, viewport, mSelected ? vec4{1.0f, 0.0f, 0.0f, 1.0f} : vec4{1.0f});

	font.begin(cam);
	font.write(choiceLeftPos, nearbySize, choiceLeftStr);
	font.write(choiceRightPos, nearbySize, choiceRightStr);
	font.end(fbo, viewport, vec4{0.5f, 0.5f, 0.5f, 1.0f});
}

// MultiChoiceSelector: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool MultiChoiceSelector::isSelected() const
{
	return mSelected;
}

bool MultiChoiceSelector::isEnabled() const
{
	return mEnabled;
}

// MultiChoiceSelector: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void MultiChoiceSelector::deselect()
{
	mSelected = false;
}

void MultiChoiceSelector::enable()
{
	mEnabled = true;
}

void MultiChoiceSelector::disable()
{
	mEnabled = false;
	mSelected = false;
}

} // namespace gui