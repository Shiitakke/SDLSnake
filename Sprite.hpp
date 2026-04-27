#pragma once
#include <string>
#include <SDL.h>

#include "common.hpp"

struct Sprite {
  SDL_Texture* _t = nullptr;
  i32 _w = 0;
  i32 _h = 0;
};
