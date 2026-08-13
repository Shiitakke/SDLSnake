#pragma once

#include <deque>
#include "common.hpp"

class World;
class SnakeGameScene;

class SnakeObject {
public:
  SnakeObject(
		World& _world,
		SnakeGameScene& _scene,
		float _speed
	);
  ~SnakeObject();
	
	enum class SnakeDir {
		UP = 1,
		DOWN = -1,
		LEFT = 2,
		RIGHT = -2,
	}; //public for snakecontroller

	void setSpeed(float new_speed);
	void updateSnake();

	SnakeDir next_dir = SnakeDir::RIGHT; //public for snakecontroller

private:
	enum class SnakeSpriteType {
		STRAIGHT,
		CURVED
	};

	enum class SnakeBoardTile {
		EMPTY,
		SNAKE
	};

	struct SnakeSegment {
		u32 id;
		BoardPos pos;
		SnakeDir in_dir;
		SnakeDir out_dir;
		SnakeSpriteType curr_type;
	};

	u32 createSnakeSegment(SnakeSegment ss);
	void destroySnakeSegment();
	
	World& world;
	SnakeGameScene& scene;
	// Every snake has its own board to track itself, as they cant collide with each other
	// Snakes check in with the appleboard to see if theres an apple wherever they are moving
	SnakeBoardTile board[16][16] = { SnakeBoardTile::EMPTY };
	std::deque<SnakeSegment> segments = {};
	float since_last_update = 0.0f;
	float speed;
	float time_per_move;
	float anim_framerate;

};

