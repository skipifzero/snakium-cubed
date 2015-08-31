#pragma once
#ifndef SFZ_GUI_TEXT_ITEM_HPP
#define SFZ_GUI_TEXT_ITEM_HPP

#include <string>

#include "sfz/gui/BaseItem.hpp"
#include "sfz/gl/Alignment.hpp"

namespace gui {

using gl::HorizontalAlign;
using std::string;

class TextItem final : public BaseItem {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	TextItem() noexcept = delete;
	TextItem(const TextItem&) noexcept = delete;
	TextItem& operator= (const TextItem&) noexcept = delete;

	TextItem(const string& text, HorizontalAlign hAlign = HorizontalAlign::CENTER) noexcept;

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
	HorizontalAlign hAlign;
};

} // namespace gui
#endif