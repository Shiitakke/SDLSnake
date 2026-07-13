#pragma once

#include <SDL.h>;

#include "Transform.hpp"

struct Camera {
  u32 id;
  SDL_Rect viewport;
  float zoom = 1.f;
};