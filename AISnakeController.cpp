#include "AISnakeController.hpp"

#include <random>

#include "SnakeObject.hpp"

AISnakeController::AISnakeController(SnakeObject& _controlled_snake) :
  controlled_snake(_controlled_snake)
{
}

void AISnakeController::update() {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<i32> directions{ 0, 3 };
  switch (directions(mt)) {
  case 0:
    controlled_snake.next_dir = SnakeObject::SnakeDir::UP;
    break;
  case 1:
    controlled_snake.next_dir = SnakeObject::SnakeDir::DOWN;
    break;
  case 2:
    controlled_snake.next_dir = SnakeObject::SnakeDir::LEFT;
    break;
  case 3:
    controlled_snake.next_dir = SnakeObject::SnakeDir::RIGHT;
    break;
  }
}