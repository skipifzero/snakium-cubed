#pragma once
#ifndef SFZ_GUI_SCROLL_LIST_CONTAINER_HPP
#define SFZ_GUI_SCROLL_LIST_CONTAINER_HPP

#include <memory>
#include <vector>

#include "sfz/gui/BaseItem.hpp"
#include "sfz/gl/Alignment.hpp"

namespace gui {

using gl::HorizontalAlign;
using std::shared_ptr;
using std::vector;

class ScrollListContainer final : public BaseItem {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ScrollListContainer(const ScrollListContainer&) = default;
	ScrollListContainer& operator= (const ScrollListContainer&) = default;

	ScrollListContainer() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool addItem(shared_ptr<BaseItem> item, vec2 dim,
	             HorizontalAlign hAlign = HorizontalAlign::CENTER) noexcept;
	bool addSpacing(float amount) noexcept;
	

	// Virtual methods overriden from BaseItem
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual bool update(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel) override final;
	virtual KeyInput update(KeyInput key) override final;
	virtual void draw(vec2 basePos, uint32_t fbo, vec2 drawableDim, const AABB2D& cam) override final;

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

	vector<shared_ptr<BaseItem>> items;

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool selectNextItemDown() noexcept;
	bool selectNextItemUp() noexcept;

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mSelected = false;
	bool mEnabled = true;
	vec2 mNextItemTopPos;
	int mCurrentSelectedIndex = -1;
};

} // namespace gui
#endif