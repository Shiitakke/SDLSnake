#pragma once
#include <SDL.h>

#include "common.hpp"
#include "vec.hpp"

struct Transform {
  u32 id;
  vec2 pos = {0, 0};
  vec2 scale = {1, 1};
  float rot = 0.f;
};