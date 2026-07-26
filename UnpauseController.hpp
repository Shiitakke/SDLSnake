#pragma once

#include "common.hpp"

#include "IUserController.hpp"
#include "SnakeGameScene.hpp"
#include "World.hpp"

class UnpauseController : public IUserController {
public:
  UnpauseController(
    SnakeGameScene& _snake, 
    World& _world, 
    u32 _pauseid
  ) : 
    snake(_snake), 
    world(_world), 
    pauseid(_pauseid)
  {
  };
  void handleEvent(const SDL_Event& e) override;
  void changePause();
private:
  u32 pauseid;
  SnakeGameScene& snake;
  World& world;
};

