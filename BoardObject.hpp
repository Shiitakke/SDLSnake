#pragma once

#include "common.hpp"
#include "vec.hpp"

class BoardObject {
public:
  enum class BoardTile {
    EMPTY,
    SNAKE,
    APPLE
  };
  
  struct BoardPos {
    i32 x = 0;
    i32 y = 0;
  };

  BoardObject() = default;
  ~BoardObject() = default;
  vec2 boardPosToWorldPos(BoardPos pos);
  
  BoardTile board[16][16];
private:
  vec2 origin;
  u32 rows = 16;
  u32 cols = 16;
  u32 width = 1000;
  u32 height = 1000;
};

