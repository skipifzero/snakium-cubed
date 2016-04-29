#include "screens/S3ItemRenderers.hpp"

#include <memory>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/gui/DefaultItemRenderers.hpp>
#include <sfz/math/Vector.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"

namespace s3 {

using gui::ItemRenderer;
using sfz::AABB2D;
using sfz::vec2;
using sfz::vec4;
using std::unique_ptr;

// snakium^3 specialized button renderer
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<Button> snakiumButtonRendererFactory() noexcept
{
	class SnakiumButtonRenderer final : public ItemRenderer {
	public:
		SnakiumButtonRenderer(Button& b) : b{b} {}
		Button& b;
		bool selectedLastFrame = false;

		virtual void update(float delta) override final
		{
			bool lastFrame = selectedLastFrame;
			bool currentFrame = b.isSelected();
			selectedLastFrame = currentFrame;

			GlobalConfig& cfg = GlobalConfig::INSTANCE();

			if (!lastFrame && currentFrame) {
				if (cfg.sfxVolume > 0) {
					Mix_Volume(-1, int32_t(std::round(cfg.sfxVolume * 12.8f)));
					Assets::INSTANCE().MENU_SELECTED_SFX.play();
				}
			}
		}

		virtual void draw(vec2 basePos, uint32_t fbo, const AABB2D& viewport, const AABB2D& cam)
		      override final
		{
			auto& settings = gui::DefaultRenderersSettings::INSTANCE();
			auto& assets = s3::Assets::INSTANCE();
			auto& sb = assets.spriteBatch;
			auto& font = assets.fontRenderer;
			
			// Render bounds if enabled
			if (settings.renderBounds) {
				sb.begin(cam);
				sb.draw(b.bounds(basePos), settings.boundsRegion);
				sb.end(fbo, viewport, settings.boundsTexture);
			}

			// Select the correct texture regions and font colors
			gl::TextureRegion* leftRegion = &assets.BUTTON_LEFT_REG;
			gl::TextureRegion* midRegion = nullptr;
			gl::TextureRegion* rightRegion = &assets.BUTTON_RIGHT_REG;
			vec4 fgColor = settings.fontColor;
			vec4 bgColor = settings.fontBgColor;
			
			if (!b.isEnabled()) {
				leftRegion = &assets.BUTTON_LEFT_DISABLED_REG;
				rightRegion = &assets.BUTTON_RIGHT_DISABLED_REG;
				fgColor = settings.fontDisabledColor;
				bgColor = settings.fontBgDisabledColor;
				//fgColor = vec4{0.80f, 0.80f, 0.80f, 1.0f};
				//bgColor = vec4{0.80f, 0.80f, 0.80f, 0.5f};
			} else if (b.isSelected()) {
				leftRegion = &assets.BUTTON_LEFT_TOUCHED_REG;
				midRegion = &assets.BUTTON_MIDDLE_TOUCHED_REG;
				rightRegion = &assets.BUTTON_RIGHT_TOUCHED_REG;
				fgColor = settings.fontSelectedColor;
				bgColor = settings.fontBgSelectedColor;
				//fgColor = vec4{0.0f, 0.0f, 0.0f, 1.0f};
				//bgColor = vec4{0.0f, 0.0f, 0.0f, 0.5f};
			}

			// Calculate position and size
			const float size = b.dim.y * settings.fontScale * 0.8f;
			vec2 pos = basePos + b.offset;
			pos.y += (b.dim.y * settings.fontVerticalOffsetScale);

			// Render the button
			sb.begin(cam);
			const auto& r = b.bounds(basePos);
			sb.draw(r.position() - vec2{r.width()/2.0f, 0.0f}, vec2{r.height()}, *leftRegion);
			if (midRegion != nullptr) {
				sb.draw(r.position(), vec2{r.width() - 0.975f*r.height(), r.height()}, *midRegion);
			}
			sb.draw(r.position() + vec2{r.width()/2.0f, 0.0f}, vec2{r.height()}, *rightRegion);
			sb.end(fbo, viewport, assets.ATLAS_128.texture());

			// Set font alignment settings
			font.horizontalAlign(gl::HorizontalAlign::CENTER);
			font.verticalAlign(gl::VerticalAlign::BOTTOM);

			// Render background text if enabled
			if (settings.fontRenderBg) {
				const vec2 bgOffs = settings.fontBgOffsetScale * size;
				font.begin(cam);
				font.write(pos + bgOffs, size, b.text.c_str());
				if (settings.fontRenderDualBg) {
					font.write(pos - bgOffs, size, b.text.c_str());
				}
				font.end(fbo, viewport, bgColor);
			}

			// Render (foreground) text
			font.begin(cam);
			font.write(pos, size, b.text.c_str());
			font.end(fbo, viewport, fgColor);
		}
	};

	return [](Button& b) {
		return unique_ptr<ItemRenderer>{new SnakiumButtonRenderer{b}};
	};
}

} // namespace s3