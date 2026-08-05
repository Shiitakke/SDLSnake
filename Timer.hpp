#pragma once

#include <functional>

#include "common.hpp"

struct Timer {
  u32 id;
  u32 interval;
  std::function<void()> callback = []()->void {};
  u32 since_last_update = 0;
  bool repeats = false;
  bool paused = false;
};