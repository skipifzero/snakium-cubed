#pragma once
#ifndef SFZ_GUI_RENDERING_SETTINGS_HPP
#define SFZ_GUI_RENDERING_SETTINGS_HPP

#include <cstdint>
#include <memory>

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/gl/FontRenderer.hpp"
#include "sfz/gl/SpriteBatch.hpp"
#include "sfz/math/Vector.hpp"

namespace gui {

using sfz::AABB2D;
using sfz::vec2;

using std::shared_ptr;
using std::uint32_t;

// Button Renderer
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Button; // Forward declare Button

class ButtonRenderer {
public:
	virtual void update(float delta) = 0;
	virtual void draw(const Button& button, vec2 basePos, uint32_t fbo, const AABB2D& viewport,
	                  const AABB2D& cam) = 0;
};

shared_ptr<ButtonRenderer> defaultButtonRenderer() noexcept;

// ImageItem Renderer
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class ImageItem; // Forward declare Button

class ImageItemRenderer {
public:
	virtual void update(float delta) = 0;
	virtual void draw(const ImageItem& imageItem, vec2 basePos, uint32_t fbo,
	                  const AABB2D& viewport, const AABB2D& cam) = 0;
};

shared_ptr<ImageItemRenderer> defaultImageItemRenderer() noexcept;

// RenderingSettings class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class RenderingSettings final {
public:
	// Singleton instance
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static RenderingSettings& INSTANCE() noexcept;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// These needs to be set manually before GUI can be rendered with default renderers
	gl::FontRenderer* fontPtr = nullptr;
	gl::SpriteBatch* spriteBatchPtr = nullptr;

	// Default renderers
	shared_ptr<ButtonRenderer> buttonRenderer = defaultButtonRenderer();
	shared_ptr<ImageItemRenderer> imageItemRenderer = defaultImageItemRenderer();

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	RenderingSettings() noexcept = default;
	RenderingSettings(const RenderingSettings&) = delete;
	RenderingSettings& operator= (const RenderingSettings&) = delete;
};

} // namespace gui
#endif