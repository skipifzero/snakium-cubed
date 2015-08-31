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

bool ImageItem::update(vec2 pointerPos, sdl::ButtonState pointerState, vec2)
{
	return false;
}

KeyInput ImageItem::update(KeyInput key)
{
	return key;
}

void ImageItem::draw(unsigned int fbo, vec2 drawableDim, vec2 camPos, vec2 camDim)
{
	auto& sb = s3::Assets::INSTANCE().spriteBatch;

	vec2 imageDim = sfz::elemMult(imageRegion.dimensions(), imageScale);
	float imageAspect = imageDim.x / imageDim.y;
	float boundsAspect = bounds.dim.x / bounds.dim.y;

	vec2 resizedImageDim;
	if (imageAspect < boundsAspect) {
		resizedImageDim.y = bounds.dim.y;
		resizedImageDim.x = bounds.dim.y * imageAspect;
	} else {
		resizedImageDim.x = bounds.dim.x;
		resizedImageDim.y = bounds.dim.x / imageAspect;
	}
	vec2 pos;
	switch (hAlign) {
	case HorizontalAlign::LEFT:
		pos = vec2{bounds.pos.x - (bounds.dim.x/2.0f) + (resizedImageDim.x/2.0f), bounds.pos.y};
		break;
	case HorizontalAlign::CENTER:
		pos = bounds.pos;
		break;
	case HorizontalAlign::RIGHT:
		pos = vec2{bounds.pos.x + (bounds.dim.x/2.0f) - (resizedImageDim.x/2.0f), bounds.pos.y};
		break;
	}

	sb.begin(camPos, camDim);
	sb.draw(pos, resizedImageDim, imageRegion);
	sb.end(fbo, drawableDim, texture);
}

void ImageItem::move(vec2 diff)
{
	bounds.pos += diff;
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