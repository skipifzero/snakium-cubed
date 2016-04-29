#pragma once
#ifndef SFZ_GUI_ITEM_RENDERER_HPP
#define SFZ_GUI_ITEM_RENDERER_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/math/Vector.hpp"

namespace gui {

using sfz::AABB2D;
using sfz::vec2;

using std::function;
using std::uint32_t;
using std::unique_ptr;

// ItemRenderer interface
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class ItemRenderer {
public:
	virtual void update(float delta) { } // Optional
	virtual void updateOnActivate() { } // Optional
	virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam) = 0;
	virtual ~ItemRenderer() = default;
};

// Common Item Rendering Factory Type
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
using ItemRendererFactory = function<unique_ptr<ItemRenderer>(T&)>;

} // namespace gui
#endif