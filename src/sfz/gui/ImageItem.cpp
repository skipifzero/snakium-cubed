#include "sfz/gui/ImageItem.hpp"

#include "sfz/Assert.hpp"
#include "sfz/gui/RenderingSettings.hpp"

namespace gui {

// ImageItem: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ImageItem::ImageItem(sfz::TextureRegion imageRegion, unsigned int texture,
                     HorizontalAlign hAlign, vec2 imageScale) noexcept
:
	imageRegion{imageRegion},
	texture{texture},
	hAlign{hAlign},
	imageScale{imageScale}
{
	renderer = RenderingSettings::INSTANCE().imageItemRendererFactory(*this);
}

// ImageItem: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ImageItem::input(vec2, vec2, sdl::ButtonState, vec2)
{
	return false;
}

KeyInput ImageItem::input(KeyInput key)
{
	return key;
}

void ImageItem::update(float delta)
{
	renderer->update(delta);
}

void ImageItem::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	renderer->draw(basePos, fbo, viewport, cam);
}

// ImageItem: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ImageItem::isSelected() const
{
	return false;
}

bool ImageItem::isEnabled() const
{
	return false;
}

// ImageItem: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ImageItem::deselect()
{
	// Nothing to do
}

void ImageItem::enable()
{
	sfz_assert_debug(false);
}

void ImageItem::disable()
{
	sfz_assert_debug(false);
}

} // namespace gui