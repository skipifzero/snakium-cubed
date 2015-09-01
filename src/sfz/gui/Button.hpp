#pragma once
#ifndef SFZ_GUI_BUTTON_HPP
#define SFZ_GUI_BUTTON_HPP

#include <functional>
#include <string>

#include "sfz/gui/BaseItem.hpp"

namespace gui {

using std::function;
using std::string;

class Button final : public BaseItem {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Button() = default;
	Button(const Button&) = default;
	Button& operator= (const Button&) = default;

	Button(const function<void(Button&)>& activateFunc) noexcept;
	Button(const string& text, const function<void(Button&)>& activateFunc);

	// Virtual methods overriden from BaseItem
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool update(vec2 pointerPos, sdl::ButtonState pointerState, vec2) override final;
	virtual KeyInput update(KeyInput key) override final;
	virtual void draw(unsigned int fbo, vec2 drawableDim, vec2 camPos, vec2 camDim) override final;
	virtual void move(vec2 diff) override final;

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
	function<void(Button&)> activateFunc = nullptr;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mSelected = false;
	bool mEnabled = true;
};

} // namespace gui
#endif