#include "sfz/gui/DefaultItemRenderers.hpp"

#include "sfz/gui/Button.hpp"
#include "sfz/gui/ImageItem.hpp"
#include "sfz/gui/MultiChoiceSelector.hpp"
#include "sfz/gui/OnOffSelector.hpp"
#include "sfz/gui/ScrollListContainer.hpp"
#include "sfz/gui/SideSplitContainer.hpp"
#include "sfz/gui/TextItem.hpp"

#include "rendering/Assets.hpp" // TODO: Remove, hilariously unportable

namespace gui {

// Default Button Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<Button> defaultButtonRendererFactory() noexcept
{
	class DefaultButtonRenderer final : public ItemRenderer {
	public:
		DefaultButtonRenderer(Button& b) : b{b} { }
		Button& b;

		virtual void update(float delta) override final
		{
			
		}

		virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
		     override final
		{
			auto& settings = DefaultRenderersSettings::INSTANCE();
			auto& sb = *settings.spriteBatchPtr;
			auto& font = *settings.fontPtr;

			// Render bounds if enabled
			if (settings.renderBounds) {
				sb.begin(cam);
				sb.draw(b.bounds(basePos), settings.boundsRegion);
				sb.end(fbo, viewport, settings.boundsTexture);
			}

			// Get font colors
			vec4 fgColor = (!b.isEnabled()) ? settings.fontDisabledColor :
			               (b.isSelected() ? settings.fontSelectedColor : settings.fontColor);
			vec4 bgColor = (!b.isEnabled()) ? settings.fontBgDisabledColor :
			               (b.isSelected() ? settings.fontBgSelectedColor : settings.fontBgColor); 
			
			// Calculate position and size
			const float size = b.dim.y * settings.fontScale;
			vec2 pos = basePos + b.offset;
			pos.y += (b.dim.y * settings.fontVerticalOffsetScale);

			// Set font alignment settings
			font.horizontalAlign(gl::HorizontalAlign::CENTER);
			font.verticalAlign(gl::VerticalAlign::BOTTOM);

			// Render background text if enabled
			if (settings.fontRenderBg) {
				const vec2 bgOffs = settings.fontBgOffsetScale * size;
				font.begin(cam);
				font.write(pos + bgOffs, size, b.text);
				if (settings.fontRenderDualBg) {
					font.write(pos - bgOffs, size, b.text);
				}
				font.end(fbo, viewport, bgColor);
			}

			// Render (foreground) text
			font.begin(cam);
			font.write(pos, size, b.text);
			font.end(fbo, viewport, fgColor);
		}
	};

	return [](Button& b) {
		return unique_ptr<ItemRenderer>{new DefaultButtonRenderer{b}};
	};
}

// Default ImageItem Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<ImageItem> defaultImageItemRendererFactory() noexcept
{
	class DefaultImageItemRenderer final : public ItemRenderer {
	public:
		DefaultImageItemRenderer(ImageItem& ii) : ii{ii} { }
		ImageItem& ii;

		virtual void update(float delta) override final
		{
			
		}

		virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
		      override final
		{
			auto& settings = DefaultRenderersSettings::INSTANCE();
			auto& sb = *settings.spriteBatchPtr;

			vec2 imageDim = ii.imageRegion.dimensions() * ii.imageScale;
			float imageAspect = imageDim.x / imageDim.y;
			float boundsAspect = ii.dim.x / ii.dim.y;

			vec2 resizedImageDim;
			if (imageAspect < boundsAspect) {
				resizedImageDim.y = ii.dim.y;
				resizedImageDim.x = ii.dim.y * imageAspect;
			} else {
				resizedImageDim.x = ii.dim.x;
				resizedImageDim.y = ii.dim.x / imageAspect;
			}

			vec2 pos = basePos + ii.offset;
			float alignSign = (float)((int8_t)ii.hAlign);
			pos.x = pos.x + (alignSign*ii.dim.x/2.0f) - (alignSign*resizedImageDim.x/2.0);

			sb.begin(cam);
			sb.draw(pos, resizedImageDim, ii.imageRegion);
			sb.end(fbo, viewport, ii.texture);
		}
	};

	return [](ImageItem& ii) {
		return unique_ptr<ItemRenderer>{new DefaultImageItemRenderer{ii}};
	};
}

// Default MultiChoiceSelector Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<MultiChoiceSelector> defaultMultiChoiceSelectorRendererFactory() noexcept
{
	class DefaultMultiChoiceSelectorRenderer final : public ItemRenderer {
	public:
		DefaultMultiChoiceSelectorRenderer(MultiChoiceSelector& mc) : mc{mc} { }
		MultiChoiceSelector& mc;

		virtual void update(float delta) override final
		{
			
		}

		virtual void draw(vec2 basePos, uint32_t fbo,
		                  const AABB2D& viewport, const AABB2D& cam) override final
		{
			auto& settings = DefaultRenderersSettings::INSTANCE();
			auto& sb = *settings.spriteBatchPtr;
			auto& font = *settings.fontPtr;

			// Render bounds if enabled
			if (settings.renderBounds) {
				sb.begin(cam);
				sb.draw(mc.bounds(basePos), settings.boundsRegion);
				sb.end(fbo, viewport, settings.boundsTexture);
			}

			// Get font colors
			vec4 fgColor = (!mc.isEnabled()) ? settings.fontDisabledColor :
			               (mc.isSelected() ? settings.fontSelectedColor : settings.fontColor);
			vec4 bgColor = (!mc.isEnabled()) ? settings.fontBgDisabledColor :
			               (mc.isSelected() ? settings.fontBgSelectedColor : settings.fontBgColor);

			// Calculate position and size
			const float size = mc.dim.y * settings.fontScale;
			const float stringWidth = font.measureStringWidth(size, mc.text);
			vec2 pos = basePos + mc.offset;
			pos.x -= (mc.dim.x/2.0f);
			pos.y += (mc.dim.y * settings.fontVerticalOffsetScale);

			// State calculations and string
			vec2 statePos = pos;
			statePos.x += std::max(mc.stateAlignOffset, font.measureStringWidth(size, mc.text + " "));
			int state = mc.checkStateFunc();
			bool stateInRange = 0 <= state && state <= ((int)mc.choiceNames.size()-1);
			static const string otherStr{"other"};
			const string& stateStr = stateInRange ? mc.choiceNames[state] : otherStr;

			// Set font alignment settings
			font.horizontalAlign(gl::HorizontalAlign::LEFT);
			font.verticalAlign(gl::VerticalAlign::BOTTOM);

			// Render background text if enabled
			if (settings.fontRenderBg) {
				const vec2 bgOffs = settings.fontBgOffsetScale * size;
				font.begin(cam);
				font.write(pos + bgOffs, size, mc.text);
				vec2 statePosCpy = statePos + bgOffs;
				if (stateInRange && state != 0) statePosCpy.x = font.write(statePosCpy, size, "< ");
				statePosCpy.x = font.write(statePosCpy, size, stateStr);
				if (stateInRange && state != ((int)mc.choiceNames.size()-1)) statePosCpy.x = font.write(statePosCpy, size, " >");
				if (settings.fontRenderDualBg) {
					font.write(pos - bgOffs, size, mc.text);
					vec2 statePosCpy2 = statePos + bgOffs;
					if (stateInRange && state != 0) statePosCpy2.x = font.write(statePosCpy2, size, "< ");
					statePosCpy2.x = font.write(statePosCpy2, size, stateStr);
					if (stateInRange && state != ((int)mc.choiceNames.size()-1)) statePosCpy2.x = font.write(statePosCpy2, size, " >");
				}
				font.end(fbo, viewport, bgColor);
			}

			// Render (foreground) text
			font.begin(cam);
			font.write(pos, size, mc.text);
			vec2 statePosCpy = statePos;
			if (stateInRange && state != 0) statePosCpy.x = font.write(statePosCpy, size, "< ");
			statePosCpy.x = font.write(statePosCpy, size, stateStr);
			if (stateInRange && state != ((int)mc.choiceNames.size()-1)) statePosCpy.x = font.write(statePosCpy, size, " >");
			font.end(fbo, viewport, fgColor);
		}
	};

	return [](MultiChoiceSelector& mc) {
		return unique_ptr<ItemRenderer>{new DefaultMultiChoiceSelectorRenderer{mc}};
	};
}

// Default OnOffSelector Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<OnOffSelector> defaultOnOffSelectorRendererFactory() noexcept
{
	class DefaultOnOffSelectorRenderer final : public ItemRenderer {
	public:
		DefaultOnOffSelectorRenderer(OnOffSelector& oo) : oo{oo} { }
		OnOffSelector& oo;

		virtual void update(float delta) override final
		{
			
		}

		virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
		     override final
		{
			using sfz::vec4;

			auto& assets = s3::Assets::INSTANCE();
			auto& sb = assets.spriteBatch;
			auto& font = assets.fontRenderer;

			sb.begin(cam);
			sb.draw(basePos + oo.offset, oo.dim, assets.TILE_FACE_REG);
			sb.end(fbo, viewport, assets.ATLAS_128.texture());

			// Font rendering preparations
			font.horizontalAlign(gl::HorizontalAlign::LEFT);
			font.verticalAlign(gl::VerticalAlign::MIDDLE);
			const float size = 0.6f * oo.dim.y;
			const float yAlignOffset = (oo.dim.y/2.0f)*0.3f;
			const float bgXAlignOffset = oo.dim.x * 0.009f;

			vec2 leftMiddlePos = basePos + oo.offset + vec2{-(oo.dim.x/2.0f), yAlignOffset};
			vec2 onPos = leftMiddlePos;
			onPos.x += std::max(oo.stateAlignOffset, font.measureStringWidth(size, oo.text + " "));
			vec2 offPos = onPos;
			offPos.x += font.measureStringWidth(size, "On ");

			bool state = false;
			if (oo.checkStateFunc) state = oo.checkStateFunc();

			// Render font shadow
			font.begin(cam);
			font.write(leftMiddlePos + vec2{bgXAlignOffset, 0.0f}, size, oo.text);
			if (state) font.write(onPos + vec2{bgXAlignOffset, 0.0f}, size, "On");
			else font.write(offPos + vec2{bgXAlignOffset, 0.0f}, size, "Off");
			font.end(fbo, viewport, vec4{0.0f, 0.0f, 0.0f, 1.0f});

			// Render button text
			font.begin(cam);
			font.write(leftMiddlePos, size, oo.text);
			font.end(fbo, viewport, oo.isSelected() ? vec4{1.0f, 0.0f, 0.0f, 1.0f} : vec4{1.0f});
			font.begin(cam);
			font.write(onPos, size, "On");
			font.end(fbo, viewport, state ? (oo.isSelected() ? vec4{1.0f, 0.0f, 0.0f, 1.0f} : vec4{1.0f}) : vec4{0.5f, 0.5f, 0.5f, 1.0f});
			font.begin(cam);
			font.write(offPos, size, "Off");
			font.end(fbo, viewport, !state ? (oo.isSelected() ? vec4{1.0f, 0.0f, 0.0f, 1.0f} : vec4{1.0f}) : vec4{0.5f, 0.5f, 0.5f, 1.0f});

		}
	};

	return [](OnOffSelector& oo) {
		return unique_ptr<ItemRenderer>{new DefaultOnOffSelectorRenderer{oo}};
	};
}

// Default ScrollListContainer Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<ScrollListContainer> defaultScrollListContainerRendererFactory() noexcept
{
	class DefaultScrollListContainerRenderer final : public ItemRenderer {
	public:
		DefaultScrollListContainerRenderer(ScrollListContainer& sl) : sl{sl} { }
		ScrollListContainer& sl;

		virtual void update(float delta) override final
		{
			
		}

		virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
		      override final
		{
			auto& settings = DefaultRenderersSettings::INSTANCE();
			auto& sb = *settings.spriteBatchPtr;

			// Render bounds if enabled
			if (settings.renderBounds) {
				sb.begin(cam);
				sb.draw(sl.bounds(basePos), settings.boundsRegion);
				sb.end(fbo, viewport, settings.boundsTexture);
			}
		}
	};

	return [](ScrollListContainer& sl) {
		return unique_ptr<ItemRenderer>{new DefaultScrollListContainerRenderer{sl}};
	};
}

// Default SideSplitContainer Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<SideSplitContainer> defaultSideSplitContainerRendererFactory() noexcept
{
	class DefaultSideSplitContainerRenderer final : public ItemRenderer {
	public:
		DefaultSideSplitContainerRenderer(SideSplitContainer& ss) : ss{ss} { }
		SideSplitContainer& ss;

		virtual void update(float delta) override final
		{

		}

		virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
		     override final
		{
			auto& settings = DefaultRenderersSettings::INSTANCE();
			auto& sb = *settings.spriteBatchPtr;

			// Render bounds if enabled
			if (settings.renderBounds) {
				sb.begin(cam);
				sb.draw(ss.bounds(basePos), settings.boundsRegion);
				sb.end(fbo, viewport, settings.boundsTexture);
			}
		}
	};

	return [](SideSplitContainer& ss) {
		return unique_ptr<ItemRenderer>{new DefaultSideSplitContainerRenderer{ss}};
	};
}

// Default TextItem Renderer Factory
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<TextItem> defaultTextItemRendererFactory() noexcept
{
	class DefaultTextItemRenderer final : public ItemRenderer {
	public:
		DefaultTextItemRenderer(TextItem& ti) : ti{ti} { }
		TextItem& ti;

		virtual void update(float delta) override final
		{

		}

		virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
		     override final
		{
			auto& settings = DefaultRenderersSettings::INSTANCE();
			auto& sb = *settings.spriteBatchPtr;
			auto& font = *settings.fontPtr;

			// Render bounds if enabled
			if (settings.renderBounds) {
				sb.begin(cam);
				sb.draw(ti.bounds(basePos), settings.boundsRegion);
				sb.end(fbo, viewport, settings.boundsTexture);
			}

			// Calculate position and size
			const float size = ti.dim.y * settings.fontScale;
			const float stringWidth = font.measureStringWidth(size, ti.text);
			vec2 pos = basePos + ti.offset;
			float alignSign = (float)(int8_t)ti.hAlign;
			pos.x += (alignSign * (ti.dim.x/2.0f));
			pos.y += (ti.dim.y * settings.fontVerticalOffsetScale);

			// Set font alignment settings
			font.horizontalAlign(ti.hAlign);
			font.verticalAlign(gl::VerticalAlign::BOTTOM);

			// Render background text if enabled
			if (settings.fontRenderBg) {
				const vec2 bgOffs = settings.fontBgOffsetScale * size;
				font.begin(cam);
				font.write(pos + bgOffs, size, ti.text);
				if (settings.fontRenderDualBg) {
					font.write(pos - bgOffs, size, ti.text);
				}
				font.end(fbo, viewport, settings.fontBgColor);
			}

			// Render (foreground) text
			font.begin(cam);
			font.write(pos, size, ti.text);
			font.end(fbo, viewport, settings.fontColor);
		}
	};

	return [](TextItem& ti) {
		return unique_ptr<ItemRenderer>{new DefaultTextItemRenderer{ti}};
	};
}

// DefaultRenderersSettings: Singleton instance
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

DefaultRenderersSettings& DefaultRenderersSettings::INSTANCE() noexcept
{
	static DefaultRenderersSettings settings;
	return settings;
}

} // namespace gui