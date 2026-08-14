#pragma once

#include "IAIController.hpp"

class SnakeObject;

class AISnakeController : public IAIController {
public:
  AISnakeController(SnakeObject& _controlled_snake);
  void update() override;
private:
  SnakeObject& controlled_snake;
};

