#pragma once

#include <functional>

#include "common.hpp"

struct Timeline {
  u32 id;
  float length;
  float curr = 0;
  std::function<void(float)> on_update = [](float)->void {};
  // curve to modify the delta coming out of the timeline, identity by default
  std::function<float(float)> delta_curve = [](float x)->float {return x;};
};