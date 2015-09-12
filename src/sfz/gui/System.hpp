#pragma once
#ifndef SFZ_GUI_SYSTEM_HPP
#define SFZ_GUI_SYSTEM_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/gl/Alignment.hpp"
#include "sfz/math/Vector.hpp"
#include "sfz/sdl/ButtonState.hpp"

#include "sfz/gui/BaseItem.hpp"
#include "sfz/gui/InputData.hpp"

namespace gui {

using gl::HorizontalAlign;

using sfz::AABB2D;
using sfz::vec2;

using std::shared_ptr;
using std::uint32_t;
using std::vector;

// System class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class System final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	System() = delete;
	System(const System&) = delete;
	System& operator= (const System&) = delete;

	System(const AABB2D& bounds);

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool addItem(shared_ptr<BaseItem> item, vec2 dim,
	             HorizontalAlign hAlign = HorizontalAlign::CENTER) noexcept;
	bool addSpacing(float amount) noexcept;

	void update(InputData data);
	void draw(uint32_t fbo, vec2 viewportDim, const AABB2D& cam);
	void draw(uint32_t fbo, const AABB2D& viewport, const AABB2D& cam);

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	inline const AABB2D& bounds() const noexcept { return mBounds; }
	inline vector<shared_ptr<BaseItem>>& items() noexcept { return mItems; }

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool selectNextItemDown() noexcept;
	bool selectNextItemUp() noexcept;

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	AABB2D mBounds;
	vector<shared_ptr<BaseItem>> mItems;
	vec2 mNextItemTopPos;
	int mCurrentSelectedIndex = -1;
};

} // namespace gui
#endif