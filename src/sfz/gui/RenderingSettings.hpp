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

// Common rendering interface
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
class ItemRenderer {
public:
	virtual void update(T& item, float delta) = 0;
	virtual void draw(T& item, vec2 basePos, uint32_t fbo, const AABB2D& viewport,
	                  const AABB2D& cam) = 0;
};

// Forward declarations
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Button;
class ImageItem;
class MultiChoiceSelector;
class OnOffSelector;
class ScrollListContainer;
class SideSplitContainer;
class TextItem;

// Default renderers
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

shared_ptr<ItemRenderer<Button>> defaultButtonRenderer() noexcept;
shared_ptr<ItemRenderer<ImageItem>> defaultImageItemRenderer() noexcept;
shared_ptr<ItemRenderer<MultiChoiceSelector>> defaultMultiChoiceSelectorRenderer() noexcept;
shared_ptr<ItemRenderer<OnOffSelector>> defaultOnOffSelectorRenderer() noexcept;
shared_ptr<ItemRenderer<ScrollListContainer>> defaultScrollListContainerRenderer() noexcept;
shared_ptr<ItemRenderer<SideSplitContainer>> defaultSideSplitContainerRenderer() noexcept;
shared_ptr<ItemRenderer<TextItem>> defaultTextItemRenderer() noexcept;

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
	shared_ptr<ItemRenderer<Button>> buttonRenderer = defaultButtonRenderer();
	shared_ptr<ItemRenderer<ImageItem>> imageItemRenderer = defaultImageItemRenderer();
	shared_ptr<ItemRenderer<MultiChoiceSelector>> multiChoiceSelectorRenderer = 
	                                                   defaultMultiChoiceSelectorRenderer();
	shared_ptr<ItemRenderer<OnOffSelector>> onOffSelectorRenderer = defaultOnOffSelectorRenderer();
	shared_ptr<ItemRenderer<ScrollListContainer>> scrollListContainerRenderer =
	                                                   defaultScrollListContainerRenderer();
	shared_ptr<ItemRenderer<SideSplitContainer>> sideSplitContainerRenderer =
	                                                  defaultSideSplitContainerRenderer();
	shared_ptr<ItemRenderer<TextItem>> textItemRenderer = defaultTextItemRenderer();

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	RenderingSettings() noexcept = default;
	RenderingSettings(const RenderingSettings&) = delete;
	RenderingSettings& operator= (const RenderingSettings&) = delete;
};

} // namespace gui
#endif