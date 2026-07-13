#pragma once

#include "common.hpp"
#include "vec.hpp"

struct LinearVelocity {
  vec2 velocity = {0, 0};
  float friction = 0.1f;
  vec2 acceleration = { 0, 0 };
};