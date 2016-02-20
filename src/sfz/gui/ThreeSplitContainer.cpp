#include "sfz/gui/ThreeSplitContainer.hpp"

#include "sfz/geometry/Intersection.hpp"
#include "sfz/gui/DefaultItemRenderers.hpp"

namespace gui {

// ThreeSplitContainer: Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<ThreeSplitContainer> ThreeSplitContainer::rendererFactory =
                                              defaultThreeSplitContainerRendererFactory();

// ThreeSplitContainer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ThreeSplitContainer::ThreeSplitContainer() noexcept
{
	renderer = rendererFactory(*this);
}

// ThreeSplitContainer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ThreeSplitContainer::setLeft(const shared_ptr<BaseItem>& item, float width,
                                  HorizontalAlign hAlign)
{
	return false;
}

bool ThreeSplitContainer::setMiddle(const shared_ptr<BaseItem>& item, float width,
                                    HorizontalAlign hAlign)
{
	return false;
}

bool ThreeSplitContainer::setRight(const shared_ptr<BaseItem>& item, float width,
                                   HorizontalAlign hAlign)
{
	return false;
}

// ThreeSplitContainer: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ThreeSplitContainer::input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	return false;
}

KeyInput ThreeSplitContainer::input(KeyInput key)
{
	return key;
}

void ThreeSplitContainer::update(float delta)
{
	renderer->update(delta);
	if (leftItem != nullptr) leftItem->update(delta);
	if (middleItem != nullptr) middleItem->update(delta);
	if (rightItem != nullptr) rightItem->update(delta);
}

void ThreeSplitContainer::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	renderer->draw(basePos, fbo, viewport, cam);
	if (leftItem != nullptr) leftItem->draw(basePos + offset, fbo, viewport, cam);
	if (middleItem != nullptr) middleItem->draw(basePos + offset, fbo, viewport, cam);
	if (rightItem != nullptr) rightItem->draw(basePos + offset, fbo, viewport, cam);
}

// ThreeSplitContainer: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ThreeSplitContainer::isSelected() const
{
	return mSelected;
}

bool ThreeSplitContainer::isEnabled() const
{
	return mEnabled;
}

// ThreeSplitContainer: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ThreeSplitContainer::deselect()
{
	mSelected = false;
	if (leftItem != nullptr) leftItem->deselect();
	if (middleItem != nullptr) middleItem->deselect();
	if (rightItem != nullptr) rightItem->deselect();
}

void ThreeSplitContainer::enable()
{
	mEnabled = true;
}

void ThreeSplitContainer::disable()
{
	mEnabled = false;
}

} // namespace gui