#pragma once

#include "common.hpp"
#include "vec.hpp"

struct AngularVelocity {
  u32 _id;
  float _velocity = 0.f;
  float _acceleration = 0.f;
  float _friction = 0.1f;
};