#include "sfz/gui/MultiChoiceSelector.hpp"

#include <new>

#include "sfz/geometry/Intersection.hpp"
#include "sfz/gui/DefaultItemRenderers.hpp"
#include "sfz/math/MathHelpers.hpp"

namespace gui {

// MultiChoiceSelector: Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<MultiChoiceSelector> MultiChoiceSelector::rendererFactory =
                                              defaultMultiChoiceSelectorRendererFactory();

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
{
	renderer = rendererFactory(*this);
}

// MultiChoiceSelector: Constructor functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MultiChoiceSelector* MultiChoiceSelector::createLinearFloat(const string& text, float* valuePtr,
                                                            float minValue, float intervalSize, size_t numIntervals, 
                                                            float stateAlignOffset, const string& unit, 
                                                            size_t numberOfDecimals)
{
	// Create the choice strings
	char format[16];
	std::snprintf(format, sizeof(format), "%s%i%s%s", "%.", int(numberOfDecimals), "f", unit.c_str());
	char buffer[256];
	vector<string> choices;
	for (size_t i = 0; i < numIntervals; ++i) {
		std::snprintf(buffer, sizeof(buffer), format, minValue + float(i) * intervalSize);
		choices.emplace_back(buffer);
	}
	
	return new (std::nothrow) MultiChoiceSelector{text, choices, [valuePtr, minValue, intervalSize, numIntervals]() {
		float val = *valuePtr;
		for (size_t i = 0; i < numIntervals; ++i) {
			if (sfz::approxEqual(val, minValue + float(i) * intervalSize)) {
				return int(i);
			}
		}
		return -1;
	}, [valuePtr, minValue, intervalSize](int choice) {
		*valuePtr = minValue + float(choice) * intervalSize;
	}, stateAlignOffset};
}

MultiChoiceSelector* MultiChoiceSelector::createLinearInteger(const string& text, int32_t* valuePtr,
                                                              int32_t minValue, int32_t intervalSize, size_t numIntervals,
                                                              float stateAlignOffset, const string& unit)
{
	// Create the choice strings
	char buffer[256];
	vector<string> choices;
	for (size_t i = 0; i < numIntervals; ++i) {
		std::snprintf(buffer, sizeof(buffer), "%i%s", minValue + int32_t(i) * intervalSize, unit.c_str());
		choices.emplace_back(buffer);
	}
	
	return new (std::nothrow) MultiChoiceSelector{text, choices, [valuePtr, minValue, intervalSize, numIntervals]() {
		int32_t val = *valuePtr;
		for (size_t i = 0; i < numIntervals; ++i) {
			if (val == (minValue + int32_t(i) * intervalSize)) {
				return int(i);
			}
		}
		return -1;
	}, [valuePtr, minValue, intervalSize](int choice) {
		*valuePtr =  minValue + int32_t(choice) * intervalSize;
	}, stateAlignOffset};
}

// MultiChoiceSelector: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool MultiChoiceSelector::input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2)
{
	if (!mEnabled) return false;
	mSelected = sfz::pointInside(bounds(basePos), pointerPos);
	if (pointerState == sdl::ButtonState::UP) {
		int currentState = checkStateFunc();
		int next = currentState + 1;
		if (currentState < 0) next = 0;
		if (next >= (int)choiceNames.size()) next = 0;
		renderer->updateOnActivate();
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
			if (0 <= currentState && currentState <= (int)choiceNames.size()-1) {
				int next = currentState + 1;
				if (next >= (int)choiceNames.size()) next = 0;
				renderer->updateOnActivate();
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
			if (next >= (int)choiceNames.size()) next = choiceNames.size() - 1;
			if (next != currentState) {
				renderer->updateOnActivate();
				changeStateFunc(next);
			}
		} else if (key == KeyInput::RIGHT) {
			int currentState = checkStateFunc();
			int next = currentState + 1;
			if (currentState < 0) next = 0;
			if (next >= (int)choiceNames.size()) next = choiceNames.size() - 1;
			if (next != currentState) {
				renderer->updateOnActivate();
				changeStateFunc(next);
			}
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
	renderer->update(delta);
}

void MultiChoiceSelector::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	renderer->draw(basePos, fbo, viewport, cam);
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