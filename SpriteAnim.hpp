#pragma once

#include <vector>

#include <SDL.h>

#include "common.hpp"

struct SpriteAnim {
  u32 _id;
  i32 _rows;
  i32 _cols;
  i32 _cellw;
  i32 _cellh;
  i32 _framerate;
  std::vector<i32> _anim_graph = {};
  float _since_last_update = 0.f;
  bool _paused = false;
  i32 _current_frame = 0;
};