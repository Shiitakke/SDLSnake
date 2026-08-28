#include "BoardObject.hpp"

// returns a vec2 to the middle of the tile in the board
vec2 BoardObject::boardPosToWorldPos (BoardPos pos) {
  vec2 start = {
    .x = (origin.x - (width / 2)),
    .y = (origin.y + (height / 2))
  };
  float tile_width = width / float(cols) ;
  float tile_height = height / float(rows);
  return {
    .x = start.x + (tile_width * pos.x) + (tile_width / 2),
    .y = start.y - (tile_height * pos.y) - (tile_height / 2)
  };
}