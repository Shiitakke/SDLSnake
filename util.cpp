#include <cmath>
#include <optional>
#include <tuple>
#include <vector>

#include <iostream>

#define PUGIXML_NO_EXCEPTIONS
#include "pugixml.hpp"

#include "util.hpp"

#include "BitmapFont.hpp"
#include "TextureManager.hpp"
#include "Transform.hpp"

// h as an angle, s and v as percentages
std::tuple<u8, u8, u8> hsvTorgb(double h, double s, double v) {
  float r;
  float g;
  float b;
  float c = v * s;
  float x = c * (1 - fabs(fmod((fmod(h, 360) / 60), 2) - 1));
  float m = v - c;

  if (0 <= h && h < 60) {
    r = c;
    g = x;
    b = 0;
  }
  else if (60 <= h && h < 120) {
    r = x;
    g = c;
    b = 0;
  }
  else if (120 <= h && h < 180) {
    r = 0;
    g = c;
    b = x;
  }
  else if (180 <= h && h < 240) {
    r = 0;
    g = x;
    b = c;
  }
  else if (240 <= h && h < 300) {
    r = x;
    g = 0;
    b = c;
  }
  else if (300 <= h && h < 360) {
    r = c;
    g = 0;
    b = x;
  }
  else {
    r = 0;
    g = 0;
    b = 0;
  }

  return {
    static_cast<u8>((r + m) * 255),
    static_cast<u8>((g + m) * 255),
    static_cast<u8>((b + m) * 255),
  };
}

// Reads an XML file in AngelCode BMFont format and returns a BMFont
// Returns an empty optional in the case of failure
// Looks for the source png file inside ./assets/fonts/{file}
std::optional<BitmapFont>loadBitmapFontFromXML(
  std::string path,
  TextureManager& texture_manager
) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.c_str());
  if (!result) {
    printf("Failed to parse bitmap font file %s\n", path.c_str());
    std::cout << "XML [" << path.c_str() << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
    std::cout << "Error description: " << result.description() << "\n";
    std::cout << "Error offset: " << result.offset << " (error at [..." << (path.c_str() + result.offset) << "]\n\n";
    return {};
  }
  // load source texture
  std::string font_source_img_path = doc.child("font").child("pages").child("page").attribute("file").value();
  if (font_source_img_path.size() == 0) {
    printf("Failed to get source img from bitmap xml");
    return {};
  }
  BitmapFont bmfont;
  // TODO: think about adding absolute path and relative path support
  // probably default to relative
  bmfont.font_path = "./assets/fonts/" + font_source_img_path;
  texture_manager.loadTexture(bmfont.font_path);
  bmfont.texture_id = texture_manager.getTextureid(bmfont.font_path);
  // setup font details
  bmfont.base = (doc.child("font").child("common").attribute("base").as_int());
  std::string spacing_string = doc.child("font").child("info").attribute("spacing").value();
  bmfont.x_spacing = std::stoi(spacing_string.substr(0, 1));
  bmfont.y_spacing = std::stoi(spacing_string.substr(2, 1));
  // setup font character details
  pugi::xml_node chars = doc.child("font").child("chars");
  for (pugi::xml_node char_node : chars) {
    BitmapChar bmchar = {
      .x = u8(char_node.attribute("x").as_uint()),
      .y = u8(char_node.attribute("y").as_uint()),
      .width = u8(char_node.attribute("width").as_uint()),
      .height = u8(char_node.attribute("height").as_uint()),
      .x_offset = i8(char_node.attribute("xoffset").as_int()),
      .y_offset = i8(char_node.attribute("yoffset").as_int()),
      .x_advance = u8(char_node.attribute("xadvance").as_uint()),
    };
    bmfont.char_map[char_node.attribute("id").as_uint()] = bmchar;
  }
  return bmfont;
}

BitmapFontWord makeBitmapWord(
  BitmapFont font, 
  World& world, 
  std::string word, 
  Transform origin, 
  Sprite format
) {
  std::vector<u32> letter_ids;
  Transform curr_letter_transform = origin;
  u32 font_texture_id = world.texture_manager.getTextureid(font.font_path);
  format.texture_id = font.texture_id;
  format.sprite_path = font.font_path;
  float x_scale = origin.scale.x;
  float y_scale = origin.scale.y;

  float x_err = 0.0;
  vec2 start_pos = curr_letter_transform.pos;
  for (char c : word) {
    BitmapChar bmchar = font.char_map[c];
    Sprite char_sprite = format;
    char_sprite.sprite_path = font.font_path;
    char_sprite.texture_id = font_texture_id;
    char_sprite.src_rect = {
      .x = (float)bmchar.x,
      .y = (float)bmchar.y,
      .w = (float)bmchar.width,
      .h = (float)bmchar.height
    };

    Transform prev_letter_transform = curr_letter_transform;
    // half offsets to account for the centering when rendering
    vec2 letter_offset = {
      .x = (bmchar.x_offset * x_scale) / 2.0f,
      .y = -((bmchar.y_offset * y_scale) / 2.0f)
    };
    letter_offset = letter_offset.rotate(curr_letter_transform.rot);
    prev_letter_transform.pos += letter_offset;
    letter_ids.emplace_back(world.ecs.createEntity(
      prev_letter_transform,
      char_sprite
    ));

    vec2 letter_advance = {
      .x = ((float)bmchar.x_advance + (font.x_spacing)) * x_scale
    };
    letter_advance = letter_advance.rotate(origin.rot);
    curr_letter_transform.pos += letter_advance;
  }
  // finally, center the whole word
  vec2 pos_delta = (curr_letter_transform.pos - start_pos) / 4.0f;
  pos_delta.y = -(font.base / 2.0f);
  for (auto letterid : letter_ids) {
    world.ecs.getc<Transform>(letterid).pos -= pos_delta;
    world.ecs.getNextc(letterid).pos -= pos_delta;
  }
  return BitmapFontWord{
    letter_ids
  };
}