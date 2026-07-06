#pragma once

#include "common.hpp"
#include "vec.hpp"

struct AngularVelocity {
  u32 id;
  float velocity = 0.f;
  float acceleration = 0.f;
  float friction = 0.1f;
};