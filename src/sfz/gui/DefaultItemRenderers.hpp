#pragma once
#ifndef SFZ_GUI_DEFAULT_ITEM_RENDERERS_HPP
#define SFZ_GUI_DEFAULT_ITEM_RENDERERS_HPP

#include <cstdint>

#include "sfz/gui/ItemRenderer.hpp"
#include "sfz/gl/FontRenderer.hpp"
#include "sfz/gl/SpriteBatch.hpp"
#include "sfz/gl/TextureRegion.hpp"
#include "sfz/math/Vector.hpp"

namespace gui {

using gl::FontRenderer;
using gl::SpriteBatch;
using gl::TextureRegion;
using sfz::vec2;
using sfz::vec4;

using std::function;
using std::uint32_t;
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
	FontRenderer* fontPtr = nullptr;
	SpriteBatch* spriteBatchPtr = nullptr;

	bool renderBounds = false;
	uint32_t boundsTexture = -1;
	TextureRegion boundsRegion{vec2{0.0f, 0.0f}, vec2{1.0f, 1.0f}};

	float fontScale = 1.0f;
	float fontVerticalOffsetScale = 0.0f;
	vec4 fontColor{1.0f, 1.0f, 1.0f, 1.0f};
	vec4 fontSelectedColor{1.0f, 0.0f, 0.0f, 1.0f};
	vec4 fontDisabledColor{0.4f, 0.4f, 0.4f, 1.0f};

	bool fontRenderBg = true;
	bool fontRenderDualBg = false;
	vec2 fontBgOffsetScale{0.02f, -0.02f};
	vec4 fontBgColor{0.0f, 0.0f, 0.0f, 1.0f};
	vec4 fontBgSelectedColor{0.0f, 0.0f, 0.0f, 1.0f};
	vec4 fontBgDisabledColor{0.0f, 0.0f, 0.0f, 1.0f};

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	DefaultRenderersSettings() noexcept = default;
	DefaultRenderersSettings(const DefaultRenderersSettings&) = delete;
	DefaultRenderersSettings& operator= (const DefaultRenderersSettings&) = delete;
};

} // namespace gui
#endif