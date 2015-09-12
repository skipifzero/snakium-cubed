#include "sfz/gui/System.hpp"

#include <iostream>

#include "sfz/geometry/Intersection.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// System: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

System::System(const AABB2D& bounds)
:
	mBounds{bounds},
	mNextItemTopPos{bounds.x(), bounds.max.y}
{ }

// System: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool System::addItem(shared_ptr<BaseItem> item, vec2 dim, HorizontalAlign hAlign) noexcept
{
	if (((mNextItemTopPos.y - dim.y) < mBounds.min.y) ||
	    (dim.x > mBounds.width())) {
		std::cerr << "gui::System: Cannot add item, out of bounds.\n";
		return false;
	}
	item->dim = dim;
	mNextItemTopPos.y -= dim.y/2.0f;
	vec2 itemPos = mNextItemTopPos;
	itemPos.x += ((float)(int8_t)hAlign)*dim.x/2.0f;
	item->offset = itemPos - mBounds.position();
	mNextItemTopPos.y -= dim.y/2.0f;
	mItems.push_back(item);
	return true;
}

bool System::addSpacing(float amount) noexcept
{
	if ((mNextItemTopPos.y - amount) < mBounds.min.y) {
		std::cerr << "gui::System: Cannot add spacing, out of bounds.\n";
		return false;
	}
	mNextItemTopPos.y -= amount;
	return true;
}

void System::update(InputData data)
{
	// Makes sure currently selected is enabled
	if (mCurrentSelectedIndex != -1) {
		if (!mItems[mCurrentSelectedIndex]->isEnabled()) {
			selectNextItemDown();
			return;
		}
	}

	// Gamepad/keys input
	if (data.key != KeyInput::NONE) {

		// Attempt to select an item if nothing is currently selected
		if (mCurrentSelectedIndex == -1) {
			if (data.key == KeyInput::UP) selectNextItemUp();
			else selectNextItemDown();
			return;
		}

		// Update current item
		KeyInput inp = data.key;
		while (inp != KeyInput::NONE) {
			
			// Update
			inp = mItems[mCurrentSelectedIndex]->update(inp);

			// Check if item is disabled
			if (!mItems[mCurrentSelectedIndex]->isEnabled()) {
				selectNextItemDown();
				inp = KeyInput::NONE;
			}

			// Select next or previous item
			if (inp == KeyInput::DOWN) {
				selectNextItemDown();
				inp = KeyInput::NONE;
			} else if (inp == KeyInput::UP) {
				selectNextItemUp();
				inp = KeyInput::NONE;
			}
		}

		return;
	}

	// Mouse/touch input
	if (data.pointerMotion != vec2{0.0f} || data.scrollWheel != vec2{0.0f}
	    || data.pointerState != sdl::ButtonState::NOT_PRESSED) {

		for (int i = 0; i < (int)mItems.size(); ++i) {

			// Skip checking disabled items
			if (!mItems[i]->isEnabled()) continue;
			
			// Check if pointer position is inside item bounds
			if (sfz::pointInside(mItems[i]->bounds(mBounds), data.pointerPos)) {
				
				// Attempt to update item
				bool success = mItems[i]->update(mBounds.position(), data.pointerPos, data.pointerState, data.scrollWheel);

				if (success) {
					
					// Deselect if not same
					if (mCurrentSelectedIndex != -1 && mCurrentSelectedIndex != i) {
						mItems[mCurrentSelectedIndex]->deselect();
					}
					mCurrentSelectedIndex = mItems[i]->isEnabled() ? i : -1; // Sets selected if enabled
				
				} else {
					
					// Deselect
					if (mCurrentSelectedIndex != -1) mItems[mCurrentSelectedIndex]->deselect();
					mCurrentSelectedIndex = -1;
				}

				return;
			}
		}

		// Nothing selected
		if (mCurrentSelectedIndex != -1) mItems[mCurrentSelectedIndex]->deselect();
		mCurrentSelectedIndex = -1;
	}
}

void System::draw(uint32_t fbo, vec2 viewportDim, const AABB2D& cam)
{
	this->draw(fbo, AABB2D{viewportDim/2.0f, viewportDim}, cam);
}

void System::draw(uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
{
	for (auto& i : mItems) i->draw(mBounds.position(), fbo, viewport, cam);
}

// System: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool System::selectNextItemDown() noexcept
{
	if (mItems.size() == 0) return false;

	// TODO: Reconsider if this is even a good idea
	if (mCurrentSelectedIndex != -1) {
		if (mItems[mCurrentSelectedIndex]->isEnabled() &&
		    mItems[mCurrentSelectedIndex]->isSelected()) {
			return true;
		}
	}

	mCurrentSelectedIndex += 1;
	if (mCurrentSelectedIndex >= (int)mItems.size()) mCurrentSelectedIndex = 0;

	for (int i = 0; i < (int)mItems.size(); ++i) {
		
		if (mItems[mCurrentSelectedIndex]->isEnabled()) {
			if (mItems[mCurrentSelectedIndex]->update(KeyInput::DOWN) == KeyInput::NONE) {
				return true;
			}
		}

		mCurrentSelectedIndex += 1;
		if (mCurrentSelectedIndex >= (int)mItems.size()) mCurrentSelectedIndex = 0;
	}

	mCurrentSelectedIndex = -1;
	return false;
}

bool System::selectNextItemUp() noexcept
{
	if (mItems.size() == 0) return false;

	// TODO: Reconsider if this is even a good idea
	if (mCurrentSelectedIndex != -1) {
		if (mItems[mCurrentSelectedIndex]->isEnabled() &&
			mItems[mCurrentSelectedIndex]->isSelected()) {
			return true;
		}
	}

	if (mCurrentSelectedIndex == -1) mCurrentSelectedIndex = mItems.size();
	mCurrentSelectedIndex -= 1;
	if (mCurrentSelectedIndex < 0) mCurrentSelectedIndex = mItems.size() - 1;

	for (int i = 0; i < (int)mItems.size(); ++i) {

		if (mItems[mCurrentSelectedIndex]->isEnabled()) {
			if (mItems[mCurrentSelectedIndex]->update(KeyInput::UP) == KeyInput::NONE) {
				return true;
			}
		}

		mCurrentSelectedIndex -= 1;
		if (mCurrentSelectedIndex < 0) mCurrentSelectedIndex = mItems.size() - 1;
	}

	mCurrentSelectedIndex = -1;
	return false;
}

} // namespace gui