#pragma once
#include <vector>

#include "common.hpp"

struct SpriteAnim {
  i32 rows;
  i32 cols;
  float cellw;
  float cellh;
  i32 framerate;
  std::vector<i32> anim_graph = {};
  float since_last_update = 0.0f;
  bool paused = false;
  i32 current_frame = 0;
};