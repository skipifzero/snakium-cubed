#include "sfz/gui/ScrollListContainer.hpp"

#include "sfz/Assert.hpp"
#include "sfz/geometry/Intersection.hpp"
#include "sfz/gui/GUIUtils.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// Static functions & variables
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const vec2 UNINITIALIZED_POS{-999.0f, -999.0f};


// ScrollListContainer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScrollListContainer::ScrollListContainer(float scrollSpeed) noexcept
:
	mScrollSpeed{scrollSpeed},
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
		std::cerr << "gui::ScrollListContainer: Cannot add item, too wide.\n";
		return false;
	}

	item->dim = dim;
	mNextItemTopPos.y -= dim.y/2.0f;
	vec2 itemPos = mNextItemTopPos;
	itemPos.x += ((float)(int8_t)hAlign)*dim.x/2.0f;
	item->offset = itemPos - offset;
	mNextItemTopPos.y -= dim.y/2.0f;
	items.push_back(item);

	mMinScrollOffset += dim.y;
	return true;
}

bool ScrollListContainer::addSpacing(float amount) noexcept
{
	if (mNextItemTopPos == UNINITIALIZED_POS) {
		mNextItemTopPos = vec2{offset.x, offset.y + (this->dim.y/2.0f)};
	}

	mNextItemTopPos.y -= amount;
	mMinScrollOffset += amount;

	return true;
}

// ScrollListContainer: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ScrollListContainer::input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	mCurrentScrollOffset -= (wheel.y * mScrollSpeed);
	mCurrentScrollOffset = std::min(mCurrentScrollOffset, mMinScrollOffset - dim.y);
	mCurrentScrollOffset = std::max(mCurrentScrollOffset, 0.0f);

	const vec2 itemBasePos = basePos + offset + vec2{0.0f, mCurrentScrollOffset};
	const AABB2D thisBounds = this->bounds(basePos);

	for (int i = 0; i < (int)items.size(); ++i) {

		// Skip checking disabled items
		if (!items[i]->isEnabled()) continue;

		AABB2D itemBounds = items[i]->bounds(itemBasePos);

		// Check if pointer position is inside item bounds
		if (sfz::overlaps(thisBounds, itemBounds) &&
			sfz::pointInside(items[i]->bounds(itemBasePos), pointerPos)) {

			// Attempt to update item
			bool success = items[i]->input(itemBasePos, pointerPos, pointerState, wheel);

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

KeyInput ScrollListContainer::input(KeyInput key)
{
	if (key == KeyInput::DOWN) {
		if (selectNextItemDown()) return KeyInput::NONE;
	} else if (key == KeyInput::UP) {
		if (selectNextItemUp()) return KeyInput::NONE;
	}

	if (key == KeyInput::ACTIVATE && mCurrentSelectedIndex != -1) {
		items[mCurrentSelectedIndex]->input(KeyInput::ACTIVATE);

		if (!items[mCurrentSelectedIndex]->isEnabled()) {
			if (!selectNextItemDown()) return KeyInput::DOWN;
		}

		return KeyInput::NONE;
	}

	if ((key == KeyInput::LEFT || key == KeyInput::RIGHT) && mCurrentSelectedIndex != -1) {
		return items[mCurrentSelectedIndex]->input(key);
	}

	return key;
}

void ScrollListContainer::update(float delta)
{
	for (auto& i : items) i->update(delta);
}

void ScrollListContainer::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	const vec2 itemBasePos = basePos + offset + vec2{0.0f, mCurrentScrollOffset};
	AABB2D thisBounds = this->bounds(basePos);

	for (auto& i : items) {
		AABB2D itemBounds = i->bounds(itemBasePos);
		if (sfz::overlaps(thisBounds, itemBounds)) {
			AABB2D thisViewport = calculateViewport(viewport, cam, thisBounds);
			i->draw(itemBasePos, fbo, thisViewport, thisBounds);
		}
	}
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
		if (items[i]->input(KeyInput::DOWN) == KeyInput::NONE) {
			mCurrentSelectedIndex = i;

			// Fix scroll offset
			const float boundsYBottom = -(dim.y/2.0f);
			const float boundsYTop = (dim.y/2.0f);
			const float itemYBottom = items[i]->offset.y + mCurrentScrollOffset - (items[i]->dim.y/2.0f);
			const float itemYTop = itemYBottom + items[i]->dim.y;
			if (itemYBottom <= boundsYBottom) {
				mCurrentScrollOffset += (boundsYBottom - itemYBottom);
			}
			if (itemYTop >= boundsYTop) {
				mCurrentScrollOffset -= (itemYTop - boundsYTop);
			}

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
		if (items[i]->input(KeyInput::DOWN) == KeyInput::NONE) {
			mCurrentSelectedIndex = i;

			// Fix scroll offset
			const float boundsYBottom = -(dim.y/2.0f);
			const float boundsYTop = (dim.y/2.0f);
			const float itemYBottom = items[i]->offset.y + mCurrentScrollOffset - (items[i]->dim.y/2.0f);
			const float itemYTop = itemYBottom + items[i]->dim.y;
			if (itemYBottom <= boundsYBottom) {
				mCurrentScrollOffset += (boundsYBottom - itemYBottom);
			}
			if (itemYTop >= boundsYTop) {
				mCurrentScrollOffset -= (itemYTop - boundsYTop);
			}

			return true;
		}
	}

	mCurrentSelectedIndex = -1;
	return false;
}

} // namespace gui