#include "sfz/gui/TextItem.hpp"

#include "sfz/Assert.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// TextItem: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TextItem::TextItem(const string& text, HorizontalAlign hAlign) noexcept
:
	text{text},
	hAlign{hAlign}
{ }

// TextItem: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TextItem::update(vec2 pointerPos, sdl::ButtonState pointerState, vec2)
{
	return false;
}

KeyInput TextItem::update(KeyInput key)
{
	return key;
}

void TextItem::draw(unsigned int fbo, vec2 drawableDim, vec2 camPos, vec2 camDim)
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

void TextItem::move(vec2 diff)
{
	bounds.pos += diff;
}

// TextItem: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TextItem::isSelected() const
{
	return false;
}

bool TextItem::isEnabled() const
{
	return false;
}

// TextItem: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TextItem::deselect()
{
	// Nothing to do
}

void TextItem::enable()
{
	sfz_assert_debug(false);
}

void TextItem::disable()
{
	sfz_assert_debug(false);
}

} // namespace gui