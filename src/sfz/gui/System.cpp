#include "sfz/gui/System.hpp"

#include "rendering/Assets.hpp" // TODO: Hilariously unportable include, remove later

namespace gui {

// System: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

System::System(const Rectangle& bounds)
:
	mBounds{bounds}
{
	
}

// System: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void System::update(InputData data)
{
	
}

void System::draw(vec2 drawableDim, vec2 guiDim, vec2 guiOffs)
{
	auto& assets = s3::Assets::INSTANCE();

	gl::SpriteBatch& sb = assets.spriteBatch;

	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	
	sb.draw(mBounds.pos, mBounds.dim, assets.TILE_FACE_REG);

	sb.end(0, drawableDim, assets.ATLAS_128.texture());
}

} // namespace gui