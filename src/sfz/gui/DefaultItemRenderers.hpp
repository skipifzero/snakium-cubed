#pragma once
#ifndef SFZ_GUI_DEFAULT_ITEM_RENDERERS_HPP
#define SFZ_GUI_DEFAULT_ITEM_RENDERERS_HPP

#include "sfz/gui/ItemRenderer.hpp"
#include "sfz/gl/FontRenderer.hpp"
#include "sfz/gl/SpriteBatch.hpp"

namespace gui {

using std::function;
using std::unique_ptr;

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

// Default renderer settings class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class DefaultRenderersSettings final {
public:
	// Singleton instance
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static DefaultRenderersSettings& INSTANCE() noexcept;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// These needs to be set manually before GUI can be rendered with default renderers
	gl::FontRenderer* fontPtr = nullptr;
	gl::SpriteBatch* spriteBatchPtr = nullptr;

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	DefaultRenderersSettings() noexcept = default;
	DefaultRenderersSettings(const DefaultRenderersSettings&) = delete;
	DefaultRenderersSettings& operator= (const DefaultRenderersSettings&) = delete;
};

} // namespace gui
#endif