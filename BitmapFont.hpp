#pragma once

#include <string>
#include <vector>

#include "common.hpp"
#include "Texture.hpp"

struct BitmapChar {
  u8 x;
  u8 y;
  u8 width;
  u8 height;
  i8 x_offset;
  i8 y_offset;
  u8 x_advance;
};

struct BitmapFont {
  u32 texture_id;
  std::string font_path;
  u32 base = 0;
  i8 x_spacing = 0;
  i8 y_spacing = 0;
  BitmapChar char_map[128];
};


// Word made from a bitmap font
struct BitmapFontWord {
  std::vector<u32> sprite_ids; // individual sprites tied to this instance
};