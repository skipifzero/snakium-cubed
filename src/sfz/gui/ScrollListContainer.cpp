#include "sfz/gui/ScrollListContainer.hpp"

#include "sfz/Assert.hpp"

#include "sfz/geometry/Intersection.hpp"

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
		mNextItemTopPos = vec2{offset.x, offset.y + (this->dim.y/2.0f)};
	}

	if (dim.x > dim.x) {
		std::cerr << "gui::System: Cannot add item, too wide.\n";
		return false;
	}
	item->dim = dim;
	mNextItemTopPos.y -= dim.y/2.0f;
	vec2 itemPos = mNextItemTopPos;
	itemPos.x += ((float)(int8_t)hAlign)*dim.x/2.0f;
	item->offset = itemPos - offset;
	mNextItemTopPos.y -= dim.y/2.0f;
	items.push_back(item);
	return true;
}

bool ScrollListContainer::addSpacing(float amount) noexcept
{
	if (mNextItemTopPos == UNINITIALIZED_POS) {
		mNextItemTopPos = vec2{offset.x, offset.y + (this->dim.y/2.0f)};
	}

	mNextItemTopPos.y -= amount;
	return true;
}

// ScrollListContainer: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScrollListContainer::update(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	for (auto& i : items) i->move(vec2{0.0f, wheel.y*10.0f}); // TODO: Remove hardcoded factor

	for (int i = 0; i < (int)items.size(); ++i) {
		
		// Skip checking disabled items
		if (!items[i]->isEnabled()) continue;

		// Check if pointer position is inside item bounds
		if (sfz::pointInside(items[i]->bounds(basePos), pointerPos)) {

			// Attempt to update item
			bool success = items[i]->update(basePos + offset, pointerPos, pointerState, wheel);

			if (success) {
				
				// Deselect if not same
				if (mCurrentSelectedIndex != -1 && mCurrentSelectedIndex != i) {
					items[mCurrentSelectedIndex]->deselect();
				}
				mCurrentSelectedIndex = items[i]->isEnabled() ? i : -1; // Sets selected if enabled

			} else {

				// Deselect
				if (mCurrentSelectedIndex != -1) items[mCurrentSelectedIndex]->deselect();
				mCurrentSelectedIndex = -1;
			}

			return mCurrentSelectedIndex != -1;
		}
	}

	// Nothing selected
	if (mCurrentSelectedIndex != -1) items[mCurrentSelectedIndex]->deselect();
	mCurrentSelectedIndex = -1;
	return false;
}

KeyInput ScrollListContainer::update(KeyInput key)
{
	if (key == KeyInput::DOWN) {
		if (selectNextItemDown()) return KeyInput::NONE;
	} else if (key == KeyInput::UP) {
		if (selectNextItemUp()) return KeyInput::NONE;
	}

	if (key == KeyInput::ACTIVATE && mCurrentSelectedIndex != -1) {
		items[mCurrentSelectedIndex]->update(KeyInput::ACTIVATE);

		if (!items[mCurrentSelectedIndex]->isEnabled()) {
			if (!selectNextItemDown()) return KeyInput::DOWN;
		}

		return KeyInput::NONE;
	}

	if ((key == KeyInput::LEFT || key == KeyInput::RIGHT) && mCurrentSelectedIndex != -1) {
		return items[mCurrentSelectedIndex]->update(key);
	}

	return key;
}

void ScrollListContainer::draw(vec2 basePos, uint32_t fbo, vec2 drawableDim, const AABB2D& cam)
{
	const float boundsYBottom = (basePos.y + offset.y) - (dim.y/2.0f);
	const float boundsYTop = boundsYBottom + dim.y;
	const vec2 conBasePos = basePos + offset;
	for (auto& i : items) {
		const float itemYBottom = conBasePos.y + i->offset.y - (i->dim.y/2.0f);
		const float itemYTop = itemYBottom + i->dim.y;
		if (boundsYBottom <= itemYBottom && itemYTop <= boundsYTop) {
			i->draw(basePos + offset, fbo, drawableDim, cam);
		}
	}
}

void ScrollListContainer::move(vec2 diff)
{
	offset += diff;
	for (auto& i : items) i->move(diff);
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
	for (auto& i : items) i->deselect();
}

void ScrollListContainer::enable()
{
	mEnabled = true;
}

void ScrollListContainer::disable()
{
	mEnabled = false;
}

// ScrollListContainer: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScrollListContainer::selectNextItemDown() noexcept
{
	if (items.size() == 0) return false;

	if (mCurrentSelectedIndex == -1) mCurrentSelectedIndex = 0;
	for (int i = mCurrentSelectedIndex; i < (int)items.size(); ++i) {

		if (!items[i]->isEnabled()) continue;
		if (items[i]->update(KeyInput::DOWN) == KeyInput::NONE) {
			mCurrentSelectedIndex = i;
			return true;
		}
	}

	mCurrentSelectedIndex = -1;
	return false;
}

bool ScrollListContainer::selectNextItemUp() noexcept
{
	if (items.size() == 0) return false;

	if (mCurrentSelectedIndex == -1) mCurrentSelectedIndex = (int)items.size()-1;
	for (int i = mCurrentSelectedIndex; i >= 0; --i) {

		if (!items[i]->isEnabled()) continue;
		if (items[i]->update(KeyInput::DOWN) == KeyInput::NONE) {
			mCurrentSelectedIndex = i;
			return true;
		}
	}

	mCurrentSelectedIndex = -1;
	return false;
}

} // namespace gui