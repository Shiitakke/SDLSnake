#pragma once

#include <string>
#include <vector>

#include "common.hpp"
#include "Texture.hpp"

struct BitmapChar {
  u8 x = 0;
  u8 y = 0;
  u8 width = 0;
  u8 height = 0;
  i8 x_offset = 0;
  i8 y_offset = 0;
  u8 x_advance = 0;
};

struct BitmapFont {
  u32 texture_id = 0;
  std::string font_path = "";
  u32 base = 0;
  i8 x_spacing = 0;
  i8 y_spacing = 0;
  BitmapChar char_map[128];
};


// Word made from a bitmap font
struct BitmapFontWord {
  std::vector<u32> sprite_ids; // individual sprites tied to this instance
};