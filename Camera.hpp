#pragma once

#include "SDL.h";

#include "Transform.hpp"

struct Camera {
  u32 _id;
  SDL_Rect _viewport;
  float _zoom = 1.f;
};