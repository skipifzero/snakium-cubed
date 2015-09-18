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

	MultiChoiceSelector(const string& text, const vector<string>& choiceNames,
	                    const function<int(void)>& checkStateFunc,
	                    const function<void(int)>& changeStateFunc,
	                    float stateAlignOffset = 0.0f);

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