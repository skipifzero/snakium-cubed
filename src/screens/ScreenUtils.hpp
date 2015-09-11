#pragma once
#ifndef S3_SCREENS_SCREEN_UTILS_HPP
#define S3_SCREENS_SCREEN_UTILS_HPP

#include <cstdint>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/gui/InputData.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/screens/BaseScreen.hpp>

namespace s3 {

using sfz::AABB2D;
using sfz::vec2;
using std::int32_t;

int32_t getFirstController(const sfz::UpdateState& state) noexcept;

gui::InputData inputDataFromUpdateState(const sfz::UpdateState& state,
                                        const AABB2D& cam, int32_t ctrlId,
                                        bool* cancelRef = nullptr) noexcept;

} // namespace s3
#endif