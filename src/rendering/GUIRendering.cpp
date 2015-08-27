#include "rendering/GUIRendering.hpp"

#include <sfz/math/Vector.hpp>

#include "Assets.hpp"

namespace s3 {

using sfz::vec2;

void renderButton(const sfz::Button& b) noexcept
{
	Assets& assets = Assets::INSTANCE();

	sfz::TextureRegion* leftRegion = &assets.BUTTON_LEFT_REG;
	sfz::TextureRegion* midRegion = nullptr;
	sfz::TextureRegion* rightRegion = &assets.BUTTON_RIGHT_REG;

	if (!b.isEnabled()) {
		leftRegion = &assets.BUTTON_LEFT_DISABLED_REG;
		rightRegion = &assets.BUTTON_RIGHT_DISABLED_REG;
	}
	else if (b.isSelected()) {
		leftRegion = &assets.BUTTON_LEFT_TOUCHED_REG;
		midRegion = &assets.BUTTON_MIDDLE_TOUCHED_REG;
		rightRegion = &assets.BUTTON_RIGHT_TOUCHED_REG;
	}

	auto& sb = assets.mSpriteBatch;
	const auto& r = b.rect();
	sb.draw(r.pos - vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *leftRegion);
	if (midRegion != nullptr) {
		sb.draw(r.pos, vec2{r.dim.x - r.dim.y, r.dim.y}, *midRegion);
	}
	sb.draw(r.pos + vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *rightRegion);
}

} // namespace s3