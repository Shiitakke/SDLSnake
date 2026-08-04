#pragma once
#include <SDL.h>
#include <vector>

#include "common.hpp"

struct SpriteAnim {
  i32 rows;
  i32 cols;
  i32 cellw;
  i32 cellh;
  i32 framerate;
  std::vector<i32> anim_graph = {};
  float since_last_update = 0.0f;
  bool paused = false;
  i32 current_frame = 0;
};