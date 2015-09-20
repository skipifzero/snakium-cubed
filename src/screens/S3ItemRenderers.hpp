#pragma once
#ifndef S3_SCREENS_S3_ITEM_RENDERERS_HPP
#define S3_SCREENS_S3_ITEM_RENDERERS_HPP

#include "sfz/gui/Button.hpp"
#include "sfz/gui/ItemRenderer.hpp"

namespace s3 {

using gui::Button;
using gui::ItemRendererFactory;

// snakium^3 specialized render factories
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ItemRendererFactory<Button> snakiumButtonRendererFactory() noexcept;

} // namespace s3
#endif