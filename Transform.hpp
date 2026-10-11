#pragma once

#include "vec.hpp"

struct Transform {
  vec2 pos = {0.0f, 0.0f};
  vec2 scale = {1.0f, 1.0f};
  float rot = 0.0f;

  bool operator==(const Transform& other) const {
    return (
      pos == other.pos &&
      scale == other.scale &&
      rot == other.rot
    );
  }
};