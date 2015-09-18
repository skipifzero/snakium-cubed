#pragma once
#ifndef SFZ_GUI_ITEM_RENDERER_HPP
#define SFZ_GUI_ITEM_RENDERER_HPP

#include <cstdint>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/math/Vector.hpp"

namespace gui {

using sfz::AABB2D;
using sfz::vec2;

using std::uint32_t;

class ItemRenderer {
public:
	virtual void update(float delta) = 0;
	virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam) = 0;
	virtual ~ItemRenderer() = default;
};

} // namespace gui
#endif