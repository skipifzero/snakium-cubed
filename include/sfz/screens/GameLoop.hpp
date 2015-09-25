#pragma once
#ifndef SFZ_SCREENS_GAME_LOOP_HPP
#define SFZ_SCREENS_GAME_LOOP_HPP

#include <memory>

#include "sfz/screens/BaseScreen.hpp"
#include "sfz/sdl/Window.hpp"

namespace sfz {

using std::shared_ptr;

void runGameLoop(sdl::Window& window, shared_ptr<BaseScreen> initialScreen);

} // namesapce sfz

#endif