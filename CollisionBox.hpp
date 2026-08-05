#pragma once

#include <functional>

#include "common.hpp"
#include "vec.hpp"

namespace CollisionChannel {
  enum Channel {
    c1 = 0b1,
    c2 = 0b10,
    c3 = 0b100
  };
}

struct CollisionBox {
  vec2 extent;
  CollisionChannel::Channel channels = CollisionChannel::c1;
  std::function<void(u32)> on_begin_collision = [](u32 otherid)->void {};
  std::function<void(u32)> on_end_collision = [](u32 otherid)->void {};
  bool colliding = false;
};