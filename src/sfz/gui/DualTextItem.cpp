#include "sfz/gui/DualTextItem.hpp"

#include "sfz/gui/DefaultItemRenderers.hpp"

namespace gui {

// DualTextItem: Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<DualTextItem> DualTextItem::rendererFactory = defaultDualTextItemRendererFactory();

// DualTextItem: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

DualTextItem::DualTextItem(const string& leftText, const string& rightText, float alignOffset,
                           HorizontalAlign leftHAlign)
:
	leftText{leftText},
	rightText{rightText},
	alignOffset{alignOffset},
	leftHAlign{leftHAlign}
{
	sfz_assert_debug(leftHAlign == HorizontalAlign::LEFT || leftHAlign == HorizontalAlign::RIGHT);
	renderer = rendererFactory(*this);
}

// DualTextItem: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool DualTextItem::input(vec2, vec2, sdl::ButtonState, vec2)
{
	return false;
}

KeyInput DualTextItem::input(KeyInput key)
{
	return key;
}

void DualTextItem::update(float delta)
{
	renderer->update(delta);
}

void DualTextItem::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	renderer->draw(basePos, fbo, viewport, cam);
}

// DualTextItem: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool DualTextItem::isSelected() const
{
	return false;
}

bool DualTextItem::isEnabled() const
{
	return false;
}

// TextItem: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void DualTextItem::deselect()
{
	// Nothing to do
}

void DualTextItem::enable()
{
	sfz_assert_debug(false);
}

void DualTextItem::disable()
{
	sfz_assert_debug(false);
}

} // namespace gui