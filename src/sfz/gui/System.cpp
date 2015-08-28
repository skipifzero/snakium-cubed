#include "sfz/gui/System.hpp"

#include <iostream>

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
	item->mBounds.dim = dim;
	if (hAlign == HorizontalAlign::CENTER) {
		item->mBounds.pos = vec2{mNextItemTopPos.x, mNextItemTopPos.y - (dim.y/2.0f)};
	} else if (hAlign == HorizontalAlign::LEFT) {
		item->mBounds.pos = vec2{mNextItemTopPos.x - (mBounds.dim.x/2.0f) + (dim.x/2.0f),
		                         mNextItemTopPos.y - (dim.y/2.0f)};
	} else if (hAlign == HorizontalAlign::RIGHT) {
		item->mBounds.pos = vec2{mNextItemTopPos.x + (mBounds.dim.x/2.0f) - (dim.x/2.0f),
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

} // namespace gui