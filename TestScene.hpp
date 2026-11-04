#pragma once

#include "IScene.hpp"

#include "BitmapFont.hpp"
#include "util.hpp"

class TestScene : public IScene {
public:
  TestScene() = default;
  ~TestScene() = default;
  void update() {
  
  };
  void initialise(World& world, TextureManager& texture_manager) {
    // setup font to use
    BitmapFont testfont2 = loadBitmapFontFromXML("./assets/fonts/CleanBasic.xml", world.texture_manager).value();
    auto letters2 = makeBitmapWord(
      testfont2,
      world,
      "abcdefghijklmnopqrstuvwxyz",
      Transform{ .pos = {.x = 0,.y = 0}, .scale = {1, 1}, .rot = 90 },
      Sprite{ .zindex = 200, .color_mod = {0 , 255, 0} }
    );
  };
  void deinitialise(World& world, TextureManager& texture_manager) {
    world.reset();
  };
};

