#include "sfz/gui/ImageItem.hpp"

#include "sfz/Assert.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

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
{ }

// ImageItem: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ImageItem::update(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	return false;
}

KeyInput ImageItem::update(KeyInput key)
{
	return key;
}

void ImageItem::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	auto& sb = s3::Assets::INSTANCE().spriteBatch;

	vec2 imageDim = imageRegion.dimensions() * imageScale;
	float imageAspect = imageDim.x / imageDim.y;
	float boundsAspect = dim.x / dim.y;

	vec2 resizedImageDim;
	if (imageAspect < boundsAspect) {
		resizedImageDim.y = dim.y;
		resizedImageDim.x = dim.y * imageAspect;
	} else {
		resizedImageDim.x = dim.x;
		resizedImageDim.y = dim.x / imageAspect;
	}

	vec2 pos = basePos + offset;
	float alignSign = (float)((int8_t)hAlign);
	pos.x = pos.x + (alignSign*dim.x/2.0f) - (alignSign*resizedImageDim.x/2.0);

	sb.begin(cam);
	sb.draw(pos, resizedImageDim, imageRegion);
	sb.end(fbo, viewport, texture);
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