#pragma once
#ifndef S3_RENDERING_RENDERING_UTILS_HPP
#define S3_RENDERING_RENDERING_UTILS_HPP

#include <cstddef>
#include <cstdint>

#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/gl/Program.hpp>
#include <sfz/gl/SimpleModel.hpp>

#include "gamelogic/Direction.hpp"
#include "gamelogic/Model.hpp"
#include "gamelogic/SnakeTile.hpp"

namespace s3 {

using gl::Program;
using gl::SimpleModel;
using sfz::mat4;
using sfz::vec3;
using std::size_t;
using std::uint32_t;

// Rendering helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SimpleModel& getTileModel(const SnakeTile* tilePtr, Direction side, float progress, bool gameOver) noexcept;

SimpleModel* getTileProjectionModelPtr(const SnakeTile* tilePtr, Direction side, float progress) noexcept;

float getTileAngleRad(Direction side, const SnakeTile* tile) noexcept;

vec3 tilePosToVector(const Model& model, const Position& tilePos) noexcept;

mat4 tileSpaceRotation(Direction side) noexcept;

uint32_t tileMaterialId(const SnakeTile* tilePtr) noexcept;

uint32_t tileDecorationMaterialId(const SnakeTile* tilePtr) noexcept;

bool isSnake(const SnakeTile* tilePtr) noexcept;

// Render functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void renderBackground(Program& program, const mat4& viewMatrix) noexcept;

void renderOpaqueSnakeProjection(const Model& model, Program& program, const mat4& viewMatrix) noexcept;

void renderCube(const Model& model, Program& program, const mat4& viewMatrix) noexcept;

void renderSnake(const Model& model, Program& program, const mat4& viewMatrix, float blurWeight) noexcept;

void renderObjects(const Model& model, Program& program, const mat4& viewMatrix) noexcept;

void renderTransparentCube(const Model& model, Program& program, const mat4& viewMatrix, vec3 camPos, size_t firstSide = 0, size_t lastSide = 5) noexcept;

void renderSnakeProjection(const Model& model, Program& program, const mat4& viewMatrix, vec3 camPos, size_t firstSide = 0, size_t lastSide = 5) noexcept;

} // namespace s3
#endif