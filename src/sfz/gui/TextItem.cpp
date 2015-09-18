#include "sfz/gui/TextItem.hpp"

#include "sfz/Assert.hpp"
#include "sfz/gui/DefaultItemRenderers.hpp"

namespace gui {

// TextItem: Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<TextItem> TextItem::rendererFactory = defaultTextItemRendererFactory();

// TextItem: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TextItem::TextItem(const string& text, HorizontalAlign hAlign) noexcept
:
	text{text},
	hAlign{hAlign}
{
	renderer = rendererFactory(*this);
}

// TextItem: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TextItem::input(vec2, vec2, sdl::ButtonState, vec2)
{
	return false;
}

KeyInput TextItem::input(KeyInput key)
{
	return key;
}

void TextItem::update(float delta)
{
	renderer->update(delta);
}

void TextItem::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	renderer->draw(basePos, fbo, viewport, cam);
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