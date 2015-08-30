#pragma once
#ifndef SFZ_GUI_BUTTON_HPP
#define SFZ_GUI_BUTTON_HPP

#include <string>

#include "sfz/gui/BaseItem.hpp"

namespace gui {

using std::string;

class Button final : public BaseItem {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Button() = delete;
	Button(const Button&) = delete;
	Button& operator= (const Button&) = delete;

	Button(void(*activateFuncPtr)(Button&) = nullptr) noexcept;
	Button(const string& text, void(*activateFuncPtr)(Button&) = nullptr) noexcept;

	// Virtual methods overriden from BaseItem
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool update(vec2 pointerPos, sdl::ButtonState pointerState, vec2) override final;
	virtual KeyInput update(KeyInput key) override final;
	virtual void draw(vec2 drawableDim, vec2 camPos, vec2 camDim) override final;
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
	void(*activateFuncPtr)(Button& ref) = nullptr;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mSelected = false;
	bool mEnabled = true;
};

} // namespace gui
#endif