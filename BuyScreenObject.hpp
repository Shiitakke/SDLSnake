#pragma once

#include "vec.hpp"

class SnakeGameScene;
class World;

class BuyScreenObject {
public:
  BuyScreenObject(
    SnakeGameScene& _scene, 
    World& _world,
    vec2 _buy_screen_origin,
    vec2 _game_screen_origin
  );
  ~BuyScreenObject();
private:
  SnakeGameScene& scene;
  World& world;
  vec2 buy_screen_origin;
  vec2 game_screen_origin;
};

