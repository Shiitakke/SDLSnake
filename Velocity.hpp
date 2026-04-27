#pragma once

#include "common.hpp"
#include "vec.hpp"

struct LinearVelocity {
  u32 _id;
  vec2 _velocity = {0, 0};
  float _friction = 0.1f;
  vec2 _acceleration = { 0, 0 };
};