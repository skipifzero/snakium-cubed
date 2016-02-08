#pragma once
#ifndef S3_SCREENS_MENU_CONSTANTS_HPP
#define S3_SCREENS_MENU_CONSTANTS_HPP

#include <memory>

#include <sfz/math/Vector.hpp>

#include <sfz/GUI.hpp>

namespace s3 {

using gui::BaseItem;
using gui::ScrollListContainer;
using gui::System;
using sfz::vec2;
using sfz::vec3;
using sfz::vec4;
using std::shared_ptr;

// Constants
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Menu dimensions
const vec2 MENU_SYSTEM_DIM = vec2{100.0f, 125.0f};
const vec2 MENU_DIM = MENU_SYSTEM_DIM - vec2{0.1f};

// Heading sizes
const float TITLE_HEIGHT = 18.0f;
const float NAVBAR_HEIGHT = 8.0f;
const float H1_HEIGHT = 7.5f;
const float H2_HEIGHT = 5.75f;
const float H3_HEIGHT = 3.8f;

// Padding
const float MENU_TOP_PADDING = 2.0f;
const float MENU_BOTTOM_PADDING = 2.0f;
const float TITLE_PADDING = 2.0f;
const float NAVBAR_PADDING = 2.0f;
const float H1_PADDING = 2.75f;
const float H2_PADDING = 2.0f;
const float H3_PADDING = 1.75f;
const float STANDARD_PADDING = 2.0f;

const float SPACE_AVAILABLE = MENU_DIM.y - TITLE_HEIGHT - NAVBAR_HEIGHT
                            - MENU_TOP_PADDING - MENU_BOTTOM_PADDING
                            - TITLE_PADDING - NAVBAR_PADDING;

// Colors
const vec3 MENU_BG_COLOR = vec3{0.2f};

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

float calcRestPadding(float numH1s, float numH2s, float numH3s, float numStandardPaddings) noexcept;

void addTitle(System& system, BaseItem* titleItem) noexcept;

void addNavbar(System& system, BaseItem* navbarItem, float restPadding, float width = MENU_DIM.x) noexcept;

void addNavbar(System& system, shared_ptr<BaseItem> navbarItem, float restPadding, float width = MENU_DIM.x) noexcept;

void addHeading1(System& system, BaseItem* item, float width = MENU_DIM.x,
                 gl::HorizontalAlign hAlign = gl::HorizontalAlign::CENTER) noexcept;

void addHeading2(System& system, BaseItem* item, float width = MENU_DIM.x,
                 gl::HorizontalAlign hAlign = gl::HorizontalAlign::CENTER) noexcept;

void addHeading3(System& system, BaseItem* item, float width = MENU_DIM.x,
                 gl::HorizontalAlign hAlign = gl::HorizontalAlign::CENTER) noexcept;

void addStandardPadding(System& system) noexcept;


void addHeading1(ScrollListContainer& scrollList, shared_ptr<BaseItem> item, float width = MENU_DIM.x,
                 gl::HorizontalAlign hAlign = gl::HorizontalAlign::CENTER) noexcept;

void addHeading2(ScrollListContainer& scrollList, shared_ptr<BaseItem> item, float width = MENU_DIM.x,
                 gl::HorizontalAlign hAlign = gl::HorizontalAlign::CENTER) noexcept;

void addHeading3(ScrollListContainer& scrollList, shared_ptr<BaseItem> item, float width = MENU_DIM.x,
                 gl::HorizontalAlign hAlign = gl::HorizontalAlign::CENTER) noexcept;



void addStandardPadding(ScrollListContainer& scrollList) noexcept;

} // namespace s3
#endif