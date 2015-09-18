#pragma once
#ifndef SFZ_GUI_RENDERING_SETTINGS_HPP
#define SFZ_GUI_RENDERING_SETTINGS_HPP

#include <functional>
#include <memory>

#include "sfz/gui/ItemRenderer.hpp"
#include "sfz/gl/FontRenderer.hpp"
#include "sfz/gl/SpriteBatch.hpp"

namespace gui {

using std::function;
using std::unique_ptr;

// Common Item Rendering Factory Type
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
using ItemRendererFactory = function<unique_ptr<ItemRenderer>(T&)>;

// Forward declarations
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Button;
class ImageItem;
class MultiChoiceSelector;
class OnOffSelector;
class ScrollListContainer;
class SideSplitContainer;
class TextItem;

// Default renderer factories
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<Button> defaultButtonRendererFactory() noexcept;
ItemRendererFactory<ImageItem> defaultImageItemRendererFactory() noexcept;
ItemRendererFactory<MultiChoiceSelector> defaultMultiChoiceSelectorRendererFactory() noexcept;
ItemRendererFactory<OnOffSelector> defaultOnOffSelectorRendererFactory() noexcept;
ItemRendererFactory<ScrollListContainer> defaultScrollListContainerRendererFactory() noexcept;
ItemRendererFactory<SideSplitContainer> defaultSideSplitContainerRendererFactory() noexcept;
ItemRendererFactory<TextItem> defaultTextItemRendererFactory() noexcept;

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

	// Renderer factories (by default the default renderers)
	ItemRendererFactory<Button> buttonRendererFactory = defaultButtonRendererFactory();
	ItemRendererFactory<ImageItem> imageItemRendererFactory = defaultImageItemRendererFactory();
	ItemRendererFactory<MultiChoiceSelector> multiChoiceSelectorRendererFactory =
	                                              defaultMultiChoiceSelectorRendererFactory();
	ItemRendererFactory<OnOffSelector> onOffSelectorRendererFactory =
	                                        defaultOnOffSelectorRendererFactory();
	ItemRendererFactory<ScrollListContainer> scrollListRendererFactory =
	                                              defaultScrollListContainerRendererFactory();
	ItemRendererFactory<SideSplitContainer> sideSplitContainerRendererFactory =
	                                             defaultSideSplitContainerRendererFactory();
	ItemRendererFactory<TextItem> textItemRendererFactory = defaultTextItemRendererFactory();

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	RenderingSettings() noexcept = default;
	RenderingSettings(const RenderingSettings&) = delete;
	RenderingSettings& operator= (const RenderingSettings&) = delete;
};

} // namespace gui
#endif