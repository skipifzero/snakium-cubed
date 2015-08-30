#include "sfz/gui/System.hpp"

#include <iostream>

#include "sfz/geometry/Intersection2D.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// System: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

System::System(const Rectangle& bounds)
:
	mBounds{bounds},
	mNextItemTopPos{bounds.pos.x, bounds.pos.y + (bounds.dim.y/2.0f)}
{
	
}

// System: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool System::addItem(shared_ptr<BaseItem> item, vec2 dim, HorizontalAlign hAlign) noexcept
{
	if (((mNextItemTopPos.y - dim.y) < (mBounds.pos.y - (mBounds.dim.y/2.0f))) ||
	    (dim.x > mBounds.dim.x)) {
		std::cerr << "gui::System: Cannot add item, out of bounds.\n";
		return false;
	}
	item->bounds.dim = dim;
	if (hAlign == HorizontalAlign::CENTER) {
		item->bounds.pos = vec2{mNextItemTopPos.x, mNextItemTopPos.y - (dim.y/2.0f)};
	} else if (hAlign == HorizontalAlign::LEFT) {
		item->bounds.pos = vec2{mNextItemTopPos.x - (mBounds.dim.x/2.0f) + (dim.x/2.0f),
		                        mNextItemTopPos.y - (dim.y/2.0f)};
	} else if (hAlign == HorizontalAlign::RIGHT) {
		item->bounds.pos = vec2{mNextItemTopPos.x + (mBounds.dim.x/2.0f) - (dim.x/2.0f),
		                        mNextItemTopPos.y - (dim.y/2.0f)};
	}
	mNextItemTopPos.y -= dim.y;
	mItems.push_back(item);
	return true;
}

bool System::addSpacing(float amount) noexcept
{
	if ((mNextItemTopPos.y - amount) < (mBounds.pos.y - (mBounds.dim.y/2.0f))) {
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
	if (data.pointerMoved || data.pointerState != sdl::ButtonState::NOT_PRESSED) {

		for (size_t i = 0; i < mItems.size(); ++i) {

			// Skip checking disabled items
			if (!mItems[i]->isEnabled()) continue;
			
			// Check if pointer position is inside item bounds
			if (sfz::pointInside(mItems[i]->bounds, data.pointerPos)) {
				
				// Attempt to update item
				bool success = mItems[i]->update(data.pointerPos, data.pointerState, data.scrollWheel);

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

void System::draw(vec2 drawableDim, vec2 camPos, vec2 camDim)
{
	auto& assets = s3::Assets::INSTANCE();
	auto& sb = assets.spriteBatch;

	sb.begin(camPos, camDim);
	sb.draw(mBounds.pos, mBounds.dim, assets.TILE_FACE_REG);
	sb.end(0, drawableDim, assets.ATLAS_128.texture());

	for (auto& m : mItems) m->draw(drawableDim, camPos, camDim);
}

// System: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool System::selectNextItemDown() noexcept
{
	if (mItems.size() == 0) return false;
	if (mCurrentSelectedIndex != -1) {
		if (mItems[mCurrentSelectedIndex]->isEnabled() &&
		    mItems[mCurrentSelectedIndex]->isSelected()) {
			return true;
		}
	}

	mCurrentSelectedIndex += 1;
	if (mCurrentSelectedIndex >= mItems.size()) mCurrentSelectedIndex = 0;

	for (int i = 0; i < mItems.size(); ++i) {
		
		if (mItems[mCurrentSelectedIndex]->isEnabled()) {
			if (mItems[mCurrentSelectedIndex]->update(KeyInput::DOWN) == KeyInput::NONE) {
				return true;
			}
		}

		mCurrentSelectedIndex += 1;
		if (mCurrentSelectedIndex >= mItems.size()) mCurrentSelectedIndex = 0;
	}

	return mCurrentSelectedIndex = -1;
	return false;
}

bool System::selectNextItemUp() noexcept
{
	if (mItems.size() == 0) return false;
	if (mCurrentSelectedIndex != -1) {
		if (mItems[mCurrentSelectedIndex]->isEnabled() &&
			mItems[mCurrentSelectedIndex]->isSelected()) {
			return true;
		}
	}

	if (mCurrentSelectedIndex == -1) mCurrentSelectedIndex = mItems.size();
	mCurrentSelectedIndex -= 1;
	if (mCurrentSelectedIndex < 0) mCurrentSelectedIndex = mItems.size() - 1;

	for (int i = 0; i < mItems.size(); ++i) {

		if (mItems[mCurrentSelectedIndex]->isEnabled()) {
			if (mItems[mCurrentSelectedIndex]->update(KeyInput::UP) == KeyInput::NONE) {
				return true;
			}
		}

		mCurrentSelectedIndex -= 1;
		if (mCurrentSelectedIndex < 0) mCurrentSelectedIndex = mItems.size() - 1;
	}

	return mCurrentSelectedIndex = -1;
	return false;
}

} // namespace gui