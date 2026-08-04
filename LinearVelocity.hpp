#pragma once

#include "vec.hpp"

struct LinearVelocity {
  vec2 velocity = { 0.0f, 0.0f };
  float friction = 0.1f;
  vec2 acceleration = { 0.0f, 0.0f };
};