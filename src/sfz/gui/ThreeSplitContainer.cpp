#include "sfz/gui/ThreeSplitContainer.hpp"

#include "sfz/geometry/Intersection.hpp"
#include "sfz/gui/DefaultItemRenderers.hpp"

namespace gui {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const int LEFT_INDEX = 0;
static const int MIDDLE_INDEX = 1;
static const int RIGHT_INDEX = 2;

static int itemLeftIndex(int current) noexcept
{
	if (current == -1) return -1;
	int index = current - 1;
	if (index < LEFT_INDEX) return -1;
	return index;
}

static int itemRightIndex(int current) noexcept
{
	if (current == -1) return -1;
	int index = current + 1;
	if (RIGHT_INDEX < index) return -1;
	return index;
}

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
	if (width > (dim.x / 3.0f)) {
		std::cerr << "gui::ThreeSplitContainer: Cannot add item, too wide.\n";
		return false;
	}

	item->dim = vec2{width, this->dim.y};
	item->offset = vec2{0.0f};
	float alignSign = static_cast<float>(static_cast<int8_t>(hAlign));
	item->offset.x = (-this->dim.x / 3.0f) + (alignSign * width / 6.0f);

	this->leftItem = item;
	return true;
}

bool ThreeSplitContainer::setMiddle(const shared_ptr<BaseItem>& item, float width,
                                    HorizontalAlign hAlign)
{
	if (width > (dim.x / 3.0f)) {
		std::cerr << "gui::ThreeSplitContainer: Cannot add item, too wide.\n";
		return false;
	}

	item->dim = vec2{width, this->dim.y};
	item->offset = vec2{0.0f};
	float alignSign = static_cast<float>(static_cast<int8_t>(hAlign));
	item->offset.x = (alignSign * width / 6.0f);

	this->middleItem = item;
	return true;
}

bool ThreeSplitContainer::setRight(const shared_ptr<BaseItem>& item, float width,
                                   HorizontalAlign hAlign)
{
	if (width > (dim.x / 3.0f)) {
		std::cerr << "gui::ThreeSplitContainer: Cannot add item, too wide.\n";
		return false;
	}

	item->dim = vec2{width, this->dim.y};
	item->offset = vec2{0.0f};
	float alignSign = static_cast<float>(static_cast<int8_t>(hAlign));
	item->offset.x = (this->dim.x / 3.0f) + (alignSign * width / 6.0f);

	this->rightItem = item;
	return true;
}

bool ThreeSplitContainer::setLeft(BaseItem* item, float width, HorizontalAlign hAlign)
{
	return this->setLeft(shared_ptr<BaseItem>{item}, width, hAlign);
}

bool ThreeSplitContainer::setMiddle(BaseItem* item, float width, HorizontalAlign hAlign)
{
	return this->setMiddle(shared_ptr<BaseItem>{item}, width, hAlign);
}

bool ThreeSplitContainer::setRight(BaseItem* item, float width, HorizontalAlign hAlign)
{
	return this->setRight(shared_ptr<BaseItem>{item}, width, hAlign);
}

// ThreeSplitContainer: Virtual methods overriden from BaseItem
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ThreeSplitContainer::input(vec2 basePos, vec2 pointerPos, sdl::ButtonState pointerState, vec2 wheel)
{
	if (!mEnabled) return false;
	if (leftItem == nullptr || middleItem == nullptr || rightItem == nullptr) return false;

	for (int i = LEFT_INDEX; i <= RIGHT_INDEX; ++i) {

		// Skip checking disabled items
		if (!item(i)->isEnabled()) continue;

		AABB2D itemBounds = item(i)->bounds(basePos + offset);

		if (sfz::pointInside(itemBounds, pointerPos)) {

			// Attempt to update item
			bool success = item(i)->input(basePos + offset, pointerPos, pointerState, wheel);

			if (success) {

				// Deselect other items
				if (i != LEFT_INDEX) leftItem->deselect();
				if (i != MIDDLE_INDEX) middleItem->deselect();
				if (i != RIGHT_INDEX) rightItem->deselect();
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

KeyInput ThreeSplitContainer::input(KeyInput key)
{
	if (!mEnabled) return key;
	if (leftItem == nullptr || middleItem == nullptr || rightItem == nullptr) return key;

	if (key == KeyInput::NONE) return key;

	// Case where no item is selected
	if (mCurrentSelectedIndex == -1) {

		// Go from right to left
		if (key == KeyInput::LEFT) {
			KeyInput res = key;
			int index = RIGHT_INDEX;
			while (index != -1) {
				if (item(index)->isEnabled()) {
					res = item(index)->input(res);
					if (res == KeyInput::NONE) {
						mCurrentSelectedIndex = index;
						return res;
					}
				}
				index = itemLeftIndex(index);
			}
		}

		// Go from left to right
		else if (key == KeyInput::DOWN || key == KeyInput::UP || key == KeyInput::RIGHT) { // Go right
			KeyInput res = key;
			int index = LEFT_INDEX;
			while (index != -1) {
				if (item(index)->isEnabled()) {
					res = item(index)->input(res);
					if (res == KeyInput::NONE) {
						mCurrentSelectedIndex = index;
						return res;
					}
				}
				index = itemRightIndex(index);
			}
		}

		// Invalid input
		return key;
	}

	// Update current item
	shared_ptr<BaseItem> currItem = this->item(mCurrentSelectedIndex);
	KeyInput res = currItem->input(key);

	// Handle potential response
	if (res == KeyInput::UP || res == KeyInput::DOWN) {
		mCurrentSelectedIndex = -1;
	}
	else if (res == KeyInput::LEFT) {
		int newIndex = itemLeftIndex(mCurrentSelectedIndex);
		mCurrentSelectedIndex = -1;
		while (newIndex != -1) {
			if (item(newIndex)->isEnabled()) {
				res = item(newIndex)->input(res);
				if (res == KeyInput::NONE) {
					mCurrentSelectedIndex = newIndex;
					break;
				}
			}
			newIndex = itemLeftIndex(newIndex);
		}
	}
	else if (res == KeyInput::RIGHT) {
		int newIndex = itemRightIndex(mCurrentSelectedIndex);
		mCurrentSelectedIndex = -1;
		while (newIndex != -1) {
			if (item(newIndex)->isEnabled()) {
				res = item(newIndex)->input(res);
				if (res == KeyInput::NONE) {
					mCurrentSelectedIndex = newIndex;
					break;
				}
			}
			newIndex = itemRightIndex(newIndex);
		}
	}
	return res;
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

// ThreeSplitContainer: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

shared_ptr<BaseItem> ThreeSplitContainer::item(int index) noexcept
{
	if (index == LEFT_INDEX) return leftItem;
	if (index == MIDDLE_INDEX) return middleItem;
	if (index == RIGHT_INDEX) return rightItem;
	sfz_assert_debug(false);
	return shared_ptr<BaseItem>{};
}

} // namespace gui