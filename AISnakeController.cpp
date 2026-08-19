#include "AISnakeController.hpp"

#include <random>
#include <queue>

#include "SnakeObject.hpp"
#include "SnakeGameScene.hpp"

AISnakeController::AISnakeController(
  SnakeObject& _controlled_snake,
  SnakeGameScene& _scene
) :
  controlled_snake(_controlled_snake),
  scene(_scene)
{
  generatePath();
}

void AISnakeController::update() {
  if (apple_path.size() < 1) {
    generatePath();
  }
  if (
    curr_head_pos.x != controlled_snake.get_head_pos().x ||
    curr_head_pos.y != controlled_snake.get_head_pos().y
  ) {
    controlled_snake.next_dir = apple_path.back();
    apple_path.pop_back();
    curr_head_pos = controlled_snake.get_head_pos();
  };
}

void AISnakeController::generatePath() {
  apple_path.clear();
  BoardObject board;
  // Scene has apples, snake has snakes
  // put apples down, overwrite with snake
  for (i32 i = 0; i < 16; ++i) {
    for (i32 j = 0; j < 16; ++j) {
      board.board[i][j] = scene.board.board[i][j];
      if (controlled_snake.board.board[i][j] == BoardObject::BoardTile::SNAKE) {
        board.board[i][j] = BoardObject::BoardTile::SNAKE;
      }
    }
  }
  bool visited[16][16] = { false };
  PathNode path[16][16] = { {0, 0, SnakeObject::SnakeDir::LEFT} };
  BoardObject::BoardPos final_pos;
  // breadth first search to apple
  bool found = false;
  std::queue<BoardObject::BoardPos> to_explore;
  BoardObject::BoardPos initial_pos = controlled_snake.get_head_pos();
  visited[initial_pos.x][initial_pos.y] = true;
  to_explore.emplace(initial_pos);
  while (to_explore.size() > 0 && !found) {
    BoardObject::BoardPos start_pos = to_explore.front();
    to_explore.pop();
    std::vector<PathNode> to_check;
    if (start_pos.x > 0) {
      BoardObject::BoardPos next_pos = start_pos;
      next_pos.x--;
      to_check.emplace_back(next_pos, SnakeObject::SnakeDir::LEFT);
    }
    if (start_pos.x < 15) {
      BoardObject::BoardPos next_pos = start_pos;
      next_pos.x++;
      to_check.emplace_back(next_pos, SnakeObject::SnakeDir::RIGHT);
    }
    if (start_pos.y > 0) {
      BoardObject::BoardPos next_pos = start_pos;
      next_pos.y--;
      to_check.emplace_back(next_pos, SnakeObject::SnakeDir::UP);
    }
    if (start_pos.y < 15) {
      BoardObject::BoardPos next_pos = start_pos;
      next_pos.y++;
      to_check.emplace_back(next_pos, SnakeObject::SnakeDir::DOWN);
    }
    for (PathNode next_node : to_check) {
      if (!visited[next_node.pos.x][next_node.pos.y]) {
        path[next_node.pos.x][next_node.pos.y] = { start_pos, next_node.dir };
        final_pos = next_node.pos;
      }
      switch (board.board[next_node.pos.x][next_node.pos.y]) {
      case BoardObject::BoardTile::EMPTY:
      {
        if (!visited[next_node.pos.x][next_node.pos.y]) {
          path[next_node.pos.x][next_node.pos.y] = { start_pos, next_node.dir };
          to_explore.emplace(next_node.pos);
          visited[next_node.pos.x][next_node.pos.y] = true;
        }
        break;
      }
      case BoardObject::BoardTile::SNAKE:
        break;
      case BoardObject::BoardTile::APPLE:
        path[next_node.pos.x][next_node.pos.y] = { start_pos, next_node.dir };
        found = true;
        final_pos = next_node.pos;
        break;
      }
    }
  }
  //if (found) {
    // Now go back along the path
    BoardObject::BoardPos curr_pos = final_pos;
    while (
      curr_pos.x != initial_pos.x ||
      curr_pos.y != initial_pos.y
    ) {
      apple_path.emplace_back(path[curr_pos.x][curr_pos.y].dir);
      curr_pos = path[curr_pos.x][curr_pos.y].pos;
    }
    curr_head_pos = initial_pos;
    controlled_snake.next_dir = apple_path.back();
    apple_path.pop_back();  
  //}
  //else {
  //  apple_path.emplace_back(SnakeObject::SnakeDir::LEFT);
  //}
}