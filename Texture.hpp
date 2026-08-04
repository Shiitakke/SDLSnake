#pragma once

#include "common.hpp"

struct Texture {
  void* t = nullptr; // static casted to needed underlying type in renderers
  i32 w = 0;
  i32 h = 0;
};
