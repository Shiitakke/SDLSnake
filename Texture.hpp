#pragma once

#include "common.hpp"

struct Texture {
  void* t = nullptr; // static casted to needed underlying type in renderers
  float w = 0;
  float h = 0;
};
