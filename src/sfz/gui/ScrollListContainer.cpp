#include "sfz/gui/ScrollListContainer.hpp"

#include "sfz/Assert.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// Static functions & variables
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const vec2 UNINITIALIZED_POS{-999.0f, -999.0f};


// ScrollListContainer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScrollListContainer::ScrollListContainer() noexcept
:
	mNextItemTopPos{UNINITIALIZED_POS}
{ }


// ScrollListContainer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScrollListContainer::addItem(shared_ptr<BaseItem> item, vec2 dim,
                                   HorizontalAlign hAlign) noexcept
{
	if (mNextItemTopPos == UNINITIALIZED_POS) {
		mNextItemTopPos = vec2{bounds.pos.x, bounds.pos.y + (bounds.dim.y/2.0f)};
	}

	if (((mNextItemTopPos.y - dim.y) < (bounds.pos.y - (bounds.dim.y/2.0f))) ||
	    (dim.x > bounds.dim.x)) {
		std::cerr << "gui::System: Cannot add item, out of bounds.\n";
		return false;
	}
	item->bounds.dim = dim;
	if (hAlign == HorizontalAlign::CENTER) {
		item->bounds.pos = vec2{mNextItemTopPos.x, mNextItemTopPos.y - (dim.y/2.0f)};
	} else if (hAlign == HorizontalAlign::LEFT) {
		item->bounds.pos = vec2{mNextItemTopPos.x - (bounds.dim.x/2.0f) + (dim.x/2.0f),
		                        mNextItemTopPos.y - (dim.y/2.0f)};
	} else if (hAlign == HorizontalAlign::RIGHT) {
		item->bounds.pos = vec2{mNextItemTopPos.x + (bounds.dim.x/2.0f) - (dim.x/2.0f),
		                        mNextItemTopPos.y - (dim.y/2.0f)};
	}
	mNextItemTopPos.y -= dim.y;
	items.push_back(item);
	return true;
}

bool ScrollListContainer::addSpacing(float amount) noexcept
{
	if (mNextItemTopPos == UNINITIALIZED_POS) {
		mNextItemTopPos = vec2{bounds.pos.x, bounds.pos.y + (bounds.dim.y/2.0f)};
	}

	if ((mNextItemTopPos.y - amount) < (bounds.pos.y - (bounds.dim.y/2.0f))) {
		std::cerr << "gui::System: Cannot add spacing, out of bounds.\n";
		return false;
	}
	mNextItemTopPos.y -= amount;
	return true;
}

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
	for (auto& m : items) m->draw(fbo, drawableDim, camPos, camDim);
}

void ScrollListContainer::move(vec2 diff)
{
	bounds.pos += diff;
}

// ScrollListContainer: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScrollListContainer::isSelected() const
{
	return mSelected;
}

bool ScrollListContainer::isEnabled() const
{
	return mEnabled;
}

// ScrollListContainer: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ScrollListContainer::deselect()
{
	mSelected = false;
}

void ScrollListContainer::enable()
{
	mEnabled = true;
}

void ScrollListContainer::disable()
{
	mEnabled = false;
}

} // namespace gui