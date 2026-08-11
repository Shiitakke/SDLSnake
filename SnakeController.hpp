#pragma once

#include "IUserController.hpp"

class SnakeObject;

class SnakeController : public IUserController {
public:
  SnakeController(SnakeObject& _controlled_snake) : controlled_snake(_controlled_snake) {}
  void handleEvent(const SDL_Event& e) override;

private:
  SnakeObject& controlled_snake;
};

