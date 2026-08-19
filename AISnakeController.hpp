#pragma once

#include "IAIController.hpp"

#include "BoardObject.hpp"
#include "SnakeObject.hpp"

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
  struct PathNode {
    BoardObject::BoardPos pos;
    SnakeObject::SnakeDir dir;
  };

  SnakeObject& controlled_snake;
  SnakeGameScene& scene;
  std::vector<SnakeObject::SnakeDir> apple_path;
  BoardObject::BoardPos curr_head_pos;
};

