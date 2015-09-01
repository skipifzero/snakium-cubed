#include "sfz/gui/ScrollListContainer.hpp"

#include "sfz/Assert.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// ScrollListContainer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScrollListContainer::ScrollListContainer() noexcept
{ }

// ScrollListContainer: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScrollListContainer::update(vec2 pointerPos, sdl::ButtonState pointerState, vec2)
{
	return false;
}

KeyInput ScrollListContainer::update(KeyInput key)
{
	return key;
}

void ScrollListContainer::draw(unsigned int fbo, vec2 drawableDim, vec2 camPos, vec2 camDim)
{
	auto& font = s3::Assets::INSTANCE().fontRenderer;

	auto& sb = s3::Assets::INSTANCE().spriteBatch;

	sb.begin(camPos, camDim);
	sb.draw(bounds.pos, bounds.dim, s3::Assets::INSTANCE().TILE_FACE_REG);
	sb.end(fbo, drawableDim, s3::Assets::INSTANCE().ATLAS_128.texture());

	float stringWidth = font.measureStringWidth(bounds.dim.y, text);
	vec2 pos;
	switch (hAlign) {
	case HorizontalAlign::LEFT:
		pos = vec2{bounds.pos.x - (bounds.dim.x/2.0f), bounds.pos.y};
		break;
	case HorizontalAlign::CENTER:
		pos = bounds.pos;
		break;
	case HorizontalAlign::RIGHT:
		pos = vec2{bounds.pos.x + (bounds.dim.x/2.0f), bounds.pos.y};
		break;
	}

	font.horizontalAlign(hAlign);
	font.verticalAlign(gl::VerticalAlign::MIDDLE);
	font.begin(camPos, camDim);
	font.write(pos, bounds.dim.y, text);
	font.end(fbo, drawableDim, sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});
}

void ScrollListContainer::move(vec2 diff)
{
	bounds.pos += diff;
}

// ScrollListContainer: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScrollListContainer::isSelected() const
{
	return false;
}

bool ScrollListContainer::isEnabled() const
{
	return false;
}

// ScrollListContainer: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ScrollListContainer::deselect()
{
	// Nothing to do
}

void ScrollListContainer::enable()
{
	sfz_assert_debug(false);
}

void ScrollListContainer::disable()
{
	sfz_assert_debug(false);
}

} // namespace gui