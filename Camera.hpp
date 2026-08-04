#pragma once

#include <SDL.h>

#include "Transform.hpp"

struct Camera {
  u32 id;
  float zoom = 1.0f;
};