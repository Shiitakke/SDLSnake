#pragma once

#include <SDL_events.h>

#include "common.hpp"
#include "IUserController.hpp"

class SnakeGameScene;
class World;

// Only responsible for pausing the scene and moving to the buy menu
class BuyController : public IUserController {
public:
  BuyController(SnakeGameScene& _game_scene, World& _world);
  void handleEvent(const SDL_Event& e) override;
private:
  void moveToBuyMenu();

  SnakeGameScene& game_scene; // For pausing the scene
  World& world; // For accessing the camera, and creating timeline/timer
};
