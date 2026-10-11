#pragma once

#include <SDL3/SDL_events.h>

#include "IUserController.hpp"
#include "vec.hpp"

class SnakeGameScene;
class World;

// Only responsible for pausing the scene and moving to the buy menu
class ShopMenuController : public IUserController {
public:
  ShopMenuController(
    SnakeGameScene& _game_scene,
    World& _world,
    vec2 _game_scene_origin
  );
  void handleEvent(const SDL_Event& e) override;
private:
  void moveToGameScreen();

  SnakeGameScene& game_scene; // For pausing the scene
  World& world; // For accessing the camera, and creating timeline/timer
  vec2 game_scene_origin;
};
