#pragma once

#include "IAIController.hpp"

class SnakeObject;
class SnakeGameScene;

class AISnakeController : public IAIController {
public:
  AISnakeController(
    SnakeObject& _controlled_snake,
    SnakeGameScene& _scene
  );
  void update() override;
  void generatePath();
private:
  SnakeObject& controlled_snake;
  SnakeGameScene& scene;
};

