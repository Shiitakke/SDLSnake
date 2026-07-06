#pragma once
#include <string>
#include <SDL.h>

#include "common.hpp"

struct Sprite {
  SDL_Texture* t = nullptr;
  i32 w = 0;
  i32 h = 0;
};
