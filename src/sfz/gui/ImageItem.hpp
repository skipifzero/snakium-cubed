#pragma once
#ifndef SFZ_GUI_IMAGE_ITEM_HPP
#define SFZ_GUI_IMAGE_ITEM_HPP

#include "sfz/gui/BaseItem.hpp"

#include "sfz/gl/Alignment.hpp"
#include "sfz/gl/TextureRegion.hpp"

namespace gui {

using gl::HorizontalAlign;

class ImageItem final : public BaseItem {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ImageItem() noexcept = delete;
	ImageItem(const ImageItem&) noexcept = delete;
	ImageItem& operator= (const ImageItem&) noexcept = delete;

	ImageItem(sfz::TextureRegion imageRegion, unsigned int texture,
	          HorizontalAlign hAlign = HorizontalAlign::CENTER,
	          vec2 imageScale = vec2{1.0f, 1.0f}) noexcept;

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
	
	sfz::TextureRegion imageRegion;
	unsigned int texture;
	HorizontalAlign hAlign;
	vec2 imageScale;
};

} // namespace gui
#endif