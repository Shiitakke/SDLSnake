#pragma once

#include "IUserController.hpp"

class SnakeObject;

class SnakeController : public IUserController {
public:
  SnakeController(SnakeObject& _controlled_snake) : controlled_snake(_controlled_snake) {}
  void handleEvent(InputAction ia) override;

private:
  SnakeObject& controlled_snake;
};

