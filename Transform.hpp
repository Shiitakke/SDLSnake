#pragma once
#include <SDL.h>

#include "common.hpp"
#include "vec.hpp"

struct Transform {
  u32 _id;
  vec2 _pos = {0, 0};
  vec2 _scale = {1, 1};
  float _rot = 0.f;
};