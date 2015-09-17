#include "sfz/gui/MultiChoiceSelector.hpp"

#include "sfz/geometry/Intersection.hpp"
#include "sfz/gui/RenderingSettings.hpp"
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
	RenderingSettings::INSTANCE().multiChoiceSelectorRenderer->update(*this, delta);
}

void MultiChoiceSelector::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	RenderingSettings::INSTANCE().multiChoiceSelectorRenderer->draw(*this, basePos, fbo, viewport, cam);
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