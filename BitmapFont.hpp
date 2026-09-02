#pragma once

#include <string>
#include <vector>

#include "common.hpp"
#include "Texture.hpp"

struct BitmapFont {
  u32 texture_id;
  std::string font_path;
  // sprte sheet cell setup
  i32 rows;
  i32 cols;
  i32 cellw;
  i32 cellh;
  u8 glyph_map[128]; // maps acsii char value to font index
};

// Word made from a bitmap font
struct BitmapFontWord {
  std::vector<u32> sprite_ids; // individual sprites tied to this instance
};