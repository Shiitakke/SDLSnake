#pragma once

#include "IUserController.hpp"
#include "SnakeGameScene.hpp"

class SnakeController : public IUserController {
public:
  SnakeController(SnakeGameScene& _controlled_snake) : controlled_snake(_controlled_snake) {}
  void handleEvent(const SDL_Event& e) override;

private:
  SnakeGameScene& controlled_snake;
};

