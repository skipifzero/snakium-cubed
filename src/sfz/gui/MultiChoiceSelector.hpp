#pragma once
#ifndef SFZ_GUI_MULTI_CHOICE_SELECTOR_HPP
#define SFZ_GUI_MULTI_CHOICE_SELECTOR_HPP

#include <functional>
#include <string>
#include <vector>

#include "sfz/gui/BaseItem.hpp"

namespace gui {

using std::function;
using std::string;
using std::vector;

/**
 * @brief A menu item for selecting between different options
 */
class MultiChoiceSelector final : public BaseItem {
public:
	// Renderer Factory (by default the default renderer)
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static ItemRendererFactory<MultiChoiceSelector> rendererFactory;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	MultiChoiceSelector() = default;
	MultiChoiceSelector(const MultiChoiceSelector&) = default;
	MultiChoiceSelector& operator= (const MultiChoiceSelector&) = default;

	/**
	 * @param text the name of the the option
	 * @param choiceNames a vector with the different possible options
	 * @param checkStateFunc a function which should return the index to the currently active choice
	 * @param changeStateFunc a function which should change the active choice to that of given index
	 * @param stateAlignOffset a offset to align choice strings against
	 */
	MultiChoiceSelector(const string& text, const vector<string>& choiceNames,
	                    const function<int(void)>& checkStateFunc,
	                    const function<void(int)>& changeStateFunc,
	                    float stateAlignOffset = 0.0f);
	
	// Constructor functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/**
	 * @brief Creates a MultiChoiceSelector for a float value with evenly placed intervals
	 * @param valuePtr the pointer to the value (for both reading and writing)
	 * @param minValue the minimum value
	 * @param intervalSize the interval between values
	 * @param unit the unit for the value
	 * @param numberOfDecimals the number of float decimals to print
	 */
	static MultiChoiceSelector* createLinearFloat(const string& text, float* valuePtr,
	                                              float minValue, float intervalSize, size_t numIntervals, 
	                                              float stateAlignOffset = 0.0f, const string& unit = "", 
	                                              size_t numberOfDecimals = 2);

	/**
	 * @brief Creates a MultiChoiceSelector for a integer value with evenly placed intervals
	 * @param text the name of the value
	 * @param valuePtr the pointer to the value (for both reading and writing)
	 * @param minValue the minimum value
	 * @param intervalSize the interval between values
	 * @param stateAlignOffset see MultiChoiceSelector constructor
	 * @param unit the unit for the value
	 */
	static MultiChoiceSelector* createLinearInteger(const string& text, int32_t* valuePtr,
	                                                int32_t minValue, int32_t intervalSize, size_t numIntervals,
	                                                float stateAlignOffset = 0.0f, const string& unit = "");

	// Virtual methods overriden from BaseItem
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel) override final;
	virtual KeyInput input(KeyInput key) override final;
	virtual void update(float delta) override final;
	virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam) override final;

	// Virtual getters overriden from BaseItem
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool isSelected() const override final;
	virtual bool isEnabled() const override final;

	// Virtual setters overriden from BaseItem
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual void deselect() override final;
	virtual void enable() override final;
	virtual void disable() override final;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	string text;
	vector<string> choiceNames;
	function<int(void)> checkStateFunc; // Should return < 0 if unknown state
	function<void(int)> changeStateFunc;
	float stateAlignOffset;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mSelected = false;
	bool mEnabled = true;
};

} // namespace gui
#endif