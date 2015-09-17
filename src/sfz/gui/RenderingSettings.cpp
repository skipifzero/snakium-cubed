#include "sfz/gui/RenderingSettings.hpp"

#include "sfz/gui/Button.hpp"
#include "sfz/gui/ImageItem.hpp"
#include "sfz/gui/MultiChoiceSelector.hpp"
#include "sfz/gui/OnOffSelector.hpp"
#include "sfz/gui/ScrollListContainer.hpp"
#include "sfz/gui/SideSplitContainer.hpp"
#include "sfz/gui/TextItem.hpp"

#include "rendering/Assets.hpp" // TODO: Remove, hilariously unportable

namespace gui {

// Default Button Renderer
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

shared_ptr<ButtonRenderer> defaultButtonRenderer() noexcept
{
	class DefaultButtonRenderer final : public ButtonRenderer {
	public:
		virtual void update(float delta) override final
		{
			
		}

		virtual void draw(const Button& b, vec2 basePos, uint32_t fbo,
		                  const AABB2D& viewport, const AABB2D& cam) override final
		{
			auto& assets = s3::Assets::INSTANCE();
			auto& sb = assets.spriteBatch;
			auto& font = assets.fontRenderer;

			/*sb.begin(camPos, camDim);
			sb.draw(mBounds.pos, mBounds.dim, assets.TILE_FACE_REG);
			sb.end(0, drawableDim, assets.ATLAS_128.texture());

			// Font rendering preparations
			font.horizontalAlign(gl::HorizontalAlign::CENTER);
			font.verticalAlign(gl::VerticalAlign::BOTTOM);
			const float size = 0.6f * mBounds.dim.y;
			const float yAlignOffset = (mBounds.dim.y/2.0f)*0.3f;
			const float bgXAlignOffset = mBounds.dim.x * 0.006f;

			// Render button text
			font.begin(camPos, camDim);
			font.write(vec2{mBounds.pos.x, mBounds.pos.y - yAlignOffset}, size, mText);
			font.end(0, drawableDim, mSelected ? sfz::vec4{1.0f, 0.0f, 0.0f, 1.0f} : sfz::vec4{1.0f});*/

			using sfz::vec4;

			// Select the correct texture regions and font colors
			sfz::TextureRegion* leftRegion = &assets.BUTTON_LEFT_REG;
			sfz::TextureRegion* midRegion = nullptr;
			sfz::TextureRegion* rightRegion = &assets.BUTTON_RIGHT_REG;
			vec4 fgColor{0.84f, 1.0f, 0.84f, 1.0f};
			vec4 bgColor{0.37f, 0.72f, 0.37f, 1.0f};

			if (!b.isEnabled()) {
				leftRegion = &assets.BUTTON_LEFT_DISABLED_REG;
				rightRegion = &assets.BUTTON_RIGHT_DISABLED_REG;
				fgColor = vec4{0.80f, 0.80f, 0.80f, 1.0f};
				bgColor = vec4{0.80f, 0.80f, 0.80f, 0.5f};
			} else if (b.isSelected()) {
				leftRegion = &assets.BUTTON_LEFT_TOUCHED_REG;
				midRegion = &assets.BUTTON_MIDDLE_TOUCHED_REG;
				rightRegion = &assets.BUTTON_RIGHT_TOUCHED_REG;
				fgColor = vec4{0.0f, 0.0f, 0.0f, 1.0f};
				bgColor = vec4{0.0f, 0.0f, 0.0f, 0.5f};
			}

			// Render the button
			sb.begin(cam.position(), cam.dimensions());
			const auto& r = b.bounds(basePos);
			sb.draw(r.position() - vec2{r.width()/2.0f, 0.0f}, vec2{r.height()}, *leftRegion);
			if (midRegion != nullptr) {
				sb.draw(r.position(), vec2{r.width() - 0.975f*r.height(), r.height()}, *midRegion);
			}
			sb.draw(r.position() + vec2{r.width()/2.0f, 0.0f}, vec2{r.height()}, *rightRegion);
			sb.end(fbo, viewport, assets.ATLAS_128.texture());

			// Font rendering preparations
			font.horizontalAlign(gl::HorizontalAlign::CENTER);
			font.verticalAlign(gl::VerticalAlign::BOTTOM);
			const float size = 0.6f * r.height();
			const float yAlignOffset = (r.height()/2.0f)*0.3f;
			const float bgXAlignOffset = size * 0.035f;

			// Render button text
			// Font shadow
			font.begin(cam.position(), cam.dimensions());
			font.write(vec2{r.x() + bgXAlignOffset, r.y() - yAlignOffset}, size, b.text);
			font.end(fbo, viewport, bgColor);
			// Font foreground
			font.begin(cam.position(), cam.dimensions());
			font.write(vec2{r.x(), r.y() - yAlignOffset}, size, b.text);
			font.end(fbo, viewport, fgColor);
		}
	};
	return shared_ptr<ButtonRenderer>{new DefaultButtonRenderer{}};
}

// Default ImageItem Renderer
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

shared_ptr<ImageItemRenderer> defaultImageItemRenderer() noexcept
{
	class DefaultImageItemRenderer final : public ImageItemRenderer {
	public:
		virtual void update(float delta) override final
		{
			
		}

		virtual void draw(const ImageItem& ii, vec2 basePos, uint32_t fbo,
		                  const AABB2D& viewport, const AABB2D& cam) override final
		{
			auto& sb = s3::Assets::INSTANCE().spriteBatch;

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
	return shared_ptr<ImageItemRenderer>{new DefaultImageItemRenderer{}};
}

// RenderingSettings: Singleton instance
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

RenderingSettings& RenderingSettings::INSTANCE() noexcept
{
	static RenderingSettings settings;
	return settings;
}

} // namespace gui