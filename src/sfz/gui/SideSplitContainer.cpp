#include "sfz/gui/SideSplitContainer.hpp"

#include "sfz/geometry/Intersection.hpp"
#include "sfz/gui/RenderingSettings.hpp"

namespace gui {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const int LEFT_INDEX = 0;
static const int RIGHT_INDEX = 1;

static int other(int current) noexcept
{
	if (current == LEFT_INDEX) return RIGHT_INDEX;
	if (current == RIGHT_INDEX) return LEFT_INDEX;
	sfz_assert_debug(false);
	return -1;
}

// SideSplitContainer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SideSplitContainer::SideSplitContainer() noexcept
{ }


// SideSplitContainer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool SideSplitContainer::setLeft(const shared_ptr<BaseItem>& item, float width,
                                 HorizontalAlign hAlign)
{
	
	if (width > (dim.x/2.0f)) {
		std::cerr << "gui::SideSplitContainer: Cannot add item, too wide.\n";
		return false;
	}

	item->dim = vec2{width, this->dim.y};
	item->offset = vec2{0.0f, 0.0f};
	float alignSign = static_cast<float>(static_cast<int8_t>(hAlign));
	item->offset.x = ((alignSign-1.0f)*this->dim.x/4.0f) - (alignSign*width/2.0f);

	this->leftItem = item;
	return true;
}

bool SideSplitContainer::setRight(const shared_ptr<BaseItem>& item, float width,
                                  HorizontalAlign hAlign)
{

	if (width > (dim.x/2.0f)) {
		std::cerr << "gui::SideSplitContainer: Cannot add item, too wide.\n";
		return false;
	}

	item->dim = vec2{width, this->dim.y};
	item->offset = vec2{0.0f, 0.0f};
	float alignSign = static_cast<float>(static_cast<int8_t>(hAlign));
	item->offset.x = ((alignSign+1.0f)*this->dim.x/4.0f) - (alignSign*width/2.0f);

	this->rightItem = item;
	return true;
}

// SideSplitContainer: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool SideSplitContainer::input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	if (!mEnabled) return false;
	if (leftItem == nullptr || rightItem == nullptr) return false;

	for (int i = LEFT_INDEX; i <= RIGHT_INDEX; ++i) {

		// Skip checking disabled items
		if (!item(i)->isEnabled()) continue;

		AABB2D itemBounds = item(i)->bounds(basePos + offset);

		if (sfz::pointInside(itemBounds, pointerPos)) {

			// Attempt to update item
			bool success = item(i)->input(basePos + offset, pointerPos, pointerState, wheel);

			if (success) {
				
				// Deselect other item if selected
				if (mCurrentSelectedIndex == other(i)) item(other(i))->deselect();
				mCurrentSelectedIndex = item(i)->isEnabled() ? i : -1; // Sets selected if enabled

			} else {

				// Deselect
				if (mCurrentSelectedIndex != -1) item(mCurrentSelectedIndex)->deselect();
				mCurrentSelectedIndex = -1;
			}

			return mCurrentSelectedIndex != -1;
		}
	}

	if (mCurrentSelectedIndex != -1) item(mCurrentSelectedIndex)->deselect();
	mCurrentSelectedIndex = -1;
	return false;
}

KeyInput SideSplitContainer::input(KeyInput key)
{
	if (!mEnabled) return key;
	if (leftItem == nullptr || rightItem == nullptr) return key;
	
	if (key == KeyInput::NONE) return key;

	// Case where no item is selected
	if (mCurrentSelectedIndex == -1) {
		if (key == KeyInput::DOWN || key == KeyInput::UP || key == KeyInput::LEFT || key == KeyInput::RIGHT) {
			shared_ptr<BaseItem> actItem = (key == KeyInput::LEFT) ? rightItem : leftItem;
			if (!actItem->isEnabled()) {
				actItem = otherItem(actItem);
				if (!actItem->isEnabled()) return key;
			}
			KeyInput res = actItem->input(key);
			if (res == KeyInput::NONE) mCurrentSelectedIndex = (actItem == leftItem) ? LEFT_INDEX : RIGHT_INDEX;
			return res;
		}
		return key;
	}

	// Update current ittem
	shared_ptr<BaseItem> currItem = this->item(mCurrentSelectedIndex);
	KeyInput res = currItem->input(key);

	// Handle potential response
	if (res != KeyInput::NONE) {

		// Check if this SideSplitContainer should be deselected
		if (res == KeyInput::UP || res == KeyInput::DOWN
		    || (res == KeyInput::LEFT && mCurrentSelectedIndex == LEFT_INDEX)
		    || (res == KeyInput::RIGHT && mCurrentSelectedIndex == RIGHT_INDEX)) {
			mCurrentSelectedIndex = -1;
		}

		if (res == KeyInput::RIGHT && mCurrentSelectedIndex == LEFT_INDEX && rightItem->isEnabled()) {
			res = rightItem->input(res);
			if (res == KeyInput::NONE) mCurrentSelectedIndex = RIGHT_INDEX;
			else mCurrentSelectedIndex = -1;
		}

		if (res == KeyInput::LEFT && mCurrentSelectedIndex == RIGHT_INDEX && leftItem->isEnabled()) {
			res = leftItem->input(res);
			if (res == KeyInput::NONE) mCurrentSelectedIndex = LEFT_INDEX;
			else mCurrentSelectedIndex = -1;
		}
	}
	return res;
}

void SideSplitContainer::update(float delta)
{
	RenderingSettings::INSTANCE().sideSplitContainerRenderer->update(*this, delta);
	if (leftItem != nullptr) leftItem->update(delta);
	if (rightItem != nullptr) rightItem->update(delta);
}

void SideSplitContainer::draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	RenderingSettings::INSTANCE().sideSplitContainerRenderer->draw(*this, basePos, fbo, viewport, cam);
	if (leftItem != nullptr) leftItem->draw(basePos + offset, fbo, viewport, cam);
	if (rightItem != nullptr) rightItem->draw(basePos + offset, fbo, viewport, cam);
}

// SideSplitContainer: Virtual getters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool SideSplitContainer::isSelected() const
{
	return mSelected;
}

bool SideSplitContainer::isEnabled() const
{
	return mEnabled;
}

// SideSplitContainer: Virtual setters overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void SideSplitContainer::deselect()
{
	mSelected = false;
	if (leftItem != nullptr) leftItem->deselect();
	if (rightItem != nullptr) rightItem->deselect();
}

void SideSplitContainer::enable()
{
	mEnabled = true;
}

void SideSplitContainer::disable()
{
	mEnabled = false;
}

// SideSplitContainer: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

shared_ptr<BaseItem> SideSplitContainer::item(int index) noexcept
{
	if (index == LEFT_INDEX) return leftItem;
	else if (index == RIGHT_INDEX) return rightItem;
	sfz_assert_debug(false);
	return nullptr;
}

shared_ptr<BaseItem> SideSplitContainer::otherItem(shared_ptr<BaseItem>& current) noexcept
{
	if (current == leftItem) return rightItem;
	if (current == rightItem) return leftItem;
	sfz_assert_debug(false);
	return shared_ptr<BaseItem>{};
}

} // namespace gui