#pragma once
#ifndef SFZ_GUI_ON_OFF_SELECTOR_HPP
#define SFZ_GUI_ON_OFF_SELECTOR_HPP

#include <functional>
#include <string>

#include "sfz/gui/BaseItem.hpp"

namespace gui {

using std::function;
using std::string;

class OnOffSelector final : public BaseItem {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	OnOffSelector() = default;
	OnOffSelector(const OnOffSelector&) = default;
	OnOffSelector& operator= (const OnOffSelector&) = default;

	OnOffSelector(const string& text, const function<bool(void)>& checkStateFunc,
	              const function<void(void)>& changeStateFunc, float stateAlignOffset = 0.0f);

	// Virtual methods overriden from BaseItem
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool update(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel) override final;
	virtual KeyInput update(KeyInput key) override final;
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
	function<bool(void)> checkStateFunc;
	function<void(void)> changeStateFunc;
	float stateAlignOffset;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mSelected = false;
	bool mEnabled = true;
};

} // namespace gui
#endif