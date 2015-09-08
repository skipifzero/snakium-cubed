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

bool TextItem::update(vec2, vec2, sdl::ButtonState, vec2)
{
	return false;
}

KeyInput TextItem::update(KeyInput key)
{
	return key;
}

void TextItem::draw(vec2 basePos, uint32_t fbo, vec2 drawableDim, const AABB2D& cam)
{
	auto& font = s3::Assets::INSTANCE().fontRenderer;

	auto& sb = s3::Assets::INSTANCE().spriteBatch;

	sb.begin(cam.position(), cam.dimensions());
	sb.draw(basePos + offset, dim, s3::Assets::INSTANCE().TILE_FACE_REG);
	sb.end(fbo, drawableDim, s3::Assets::INSTANCE().ATLAS_128.texture());

	float stringWidth = font.measureStringWidth(dim.y, text);
	vec2 pos = basePos + offset;
	float alignSign = (float)(int8_t)hAlign;
	pos.x += (alignSign*(dim.x/2.0f));
	
	font.horizontalAlign(hAlign);
	font.verticalAlign(gl::VerticalAlign::MIDDLE);
	font.begin(cam.position(), cam.dimensions());
	font.write(pos, dim.y, text);
	font.end(fbo, drawableDim, sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});
}

void TextItem::move(vec2 diff)
{
	offset += diff;
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