#pragma once

#include <functional>

#include "common.hpp"

struct Timeline {
  u32 id;
  float length;
  float curr = 0.0f;
  std::function<void(float)> on_update = [](float)->void {};
  std::function<void()> on_end = []()->void {}; // no delta, should always be 1 at this point
  // curve to modify the delta coming out of the timeline, identity by default
  std::function<float(float)> delta_curve = [](float x)->float {return x;};
};