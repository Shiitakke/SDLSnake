#pragma once
#include <SDL.h>

#include "vec.hpp"

struct Transform {
  vec2 pos = {0.0f, 0.0f};
  vec2 scale = {1.0f, 1.0f};
  float rot = 0.0f;
};