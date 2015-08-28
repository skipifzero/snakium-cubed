#include "rendering/GUIRendering.hpp"

#include <sfz/math/Vector.hpp>

#include "Assets.hpp"
#include "screens/ScreenMenuConstants.hpp"

namespace s3 {

/*void renderButton(const Button& b, vec2 drawableDim, vec2 guiDim, vec2 guiOffs) noexcept
{
	Assets& assets = Assets::INSTANCE();
	gl::SpriteBatch& sb = assets.spriteBatch;
	gl::FontRenderer& font = assets.fontRenderer;

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
	}
	else if (b.isSelected()) {
		leftRegion = &assets.BUTTON_LEFT_TOUCHED_REG;
		midRegion = &assets.BUTTON_MIDDLE_TOUCHED_REG;
		rightRegion = &assets.BUTTON_RIGHT_TOUCHED_REG;
		fgColor = vec4{0.0f, 0.0f, 0.0f, 1.0f};
		bgColor = vec4{0.0f, 0.0f, 0.0f, 0.5f};
	}

	// Render the button
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	const auto& r = b.rect();
	sb.draw(r.pos - vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *leftRegion);
	if (midRegion != nullptr) {
		sb.draw(r.pos, vec2{r.dim.x - 0.975f*r.dim.y, r.dim.y}, *midRegion);
	}
	sb.draw(r.pos + vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *rightRegion);
	sb.end(0, drawableDim, assets.ATLAS_128.texture());

	// Font rendering preparations
	font.horizontalAlign(gl::HorizontalAlign::CENTER);
	font.verticalAlign(gl::VerticalAlign::BOTTOM);
	const float size = 0.6f * r.dim.y;
	const float yAlignOffset = (r.dim.y/2.0f)*0.3f;
	const float bgXAlignOffset = r.dim.x * 0.006f;

	// Render button text
	// Font shadow
	font.begin(guiOffs + (guiDim/2.0f), guiDim);
	font.write(vec2{r.pos.x + bgXAlignOffset, r.pos.y - yAlignOffset}, size, b.text());
	font.end(0, drawableDim, bgColor);
	// Font foreground
	font.begin(guiOffs + (guiDim/2.0f), guiDim);
	font.write(vec2{r.pos.x, r.pos.y - yAlignOffset}, size, b.text());
	font.end(0, drawableDim, fgColor);
}*/

} // namespace s3