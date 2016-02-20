#pragma once
#ifndef SFZ_GUI_THREE_SPLIT_CONTAINER_HPP
#define SFZ_GUI_THREE_SPLIT_CONTAINER_HPP

#include <memory>
#include <vector>

#include "sfz/gui/BaseItem.hpp"
#include "sfz/gl/Alignment.hpp"

namespace gui {

using gl::HorizontalAlign;
using std::shared_ptr;

class ThreeSplitContainer final : public BaseItem {
public:
	// Renderer Factory (by default the default renderer)
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static ItemRendererFactory<ThreeSplitContainer> rendererFactory;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ThreeSplitContainer(const ThreeSplitContainer&) = default;
	ThreeSplitContainer& operator= (const ThreeSplitContainer&) = default;

	ThreeSplitContainer() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool setLeft(const shared_ptr<BaseItem>& item, float width,
	             HorizontalAlign hAlign = HorizontalAlign::CENTER);
	bool setMiddle(const shared_ptr<BaseItem>& item, float width,
	               HorizontalAlign hAlign = HorizontalAlign::CENTER);
	bool setRight(const shared_ptr<BaseItem>& item, float width,
	              HorizontalAlign hAlign = HorizontalAlign::CENTER);

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

	shared_ptr<BaseItem> leftItem, middleItem, rightItem;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mSelected = false;
	bool mEnabled = true;
	int mCurrentSelectedIndex = -1;
};

} // namespace gui
#endif