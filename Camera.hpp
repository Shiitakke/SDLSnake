#pragma once

#include "Transform.hpp"

struct Camera {
  float zoom = 1.0f;
  Transform curr;
  Transform next;
};