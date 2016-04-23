#pragma once
#ifndef SFZ_GUI_IMAGE_ITEM_HPP
#define SFZ_GUI_IMAGE_ITEM_HPP

#include "sfz/gui/BaseItem.hpp"

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/gl/Alignment.hpp"
#include "sfz/gl/TextureRegion.hpp"
#include "sfz/gl/Texture.hpp"

namespace gui {

using gl::HorizontalAlign;
using sfz::AABB2D;

class ImageItem final : public BaseItem {
public:
	// Renderer Factory (by default the default renderer)
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static ItemRendererFactory<ImageItem> rendererFactory;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ImageItem() noexcept = delete;
	ImageItem(const ImageItem&) noexcept = delete;
	ImageItem& operator= (const ImageItem&) noexcept = delete;

	ImageItem(gl::TextureRegion imageRegion, unsigned int texture, const AABB2D& imageDim,
	          HorizontalAlign hAlign = HorizontalAlign::CENTER,
	          vec2 imageScale = vec2{1.0f, 1.0f}) noexcept;

	ImageItem(const gl::Texture& texture,
	          HorizontalAlign hAlign = HorizontalAlign::CENTER,
	          vec2 imageScale = vec2{1.0f, 1.0f}) noexcept;

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
	
	gl::TextureRegion imageRegion;
	AABB2D imageDim;
	unsigned int texture;
	HorizontalAlign hAlign;
	vec2 imageScale;
};

} // namespace gui
#endif