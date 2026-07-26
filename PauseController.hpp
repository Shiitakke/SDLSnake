#pragma once

#include "common.hpp"

#include "IUserController.hpp"
#include "SnakeGameScene.hpp"
#include "World.hpp"

class PauseController : public IUserController {
public:
  PauseController(SnakeGameScene& _snake, World& _world) : snake(_snake), world(_world) {};
  void handleEvent(const SDL_Event& e) override;
  void changePause();
private:
  u32 pause_id = 0;
  bool is_paused = false;
  SnakeGameScene& snake;
  World& world;
};

