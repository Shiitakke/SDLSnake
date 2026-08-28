#pragma once

#include <deque>

#include "BoardObject.hpp"
#include "common.hpp"
#include "Sprite.hpp"

class SnakeGameScene;
class World;

class SnakeObject {
public:
  SnakeObject(
		World& _world,
		SnakeGameScene& _scene,
		float _speed,
		SpriteColorMod _snake_color
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
	BoardObject::BoardPos get_head_pos();
	void killSnake();
	void init();

	SnakeDir next_dir = SnakeDir::RIGHT; //public for snakecontroller
	BoardObject board; // public for ai snake controller
private:
	enum class SnakeSpriteType {
		STRAIGHT,
		CURVED
	};

	struct SnakeSegment {
		u32 id;
		BoardObject::BoardPos pos;
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
	std::deque<SnakeSegment> segments = {};
	SpriteColorMod snake_color;
	u32 head_collision_id; // 
	float since_last_update = 0.0f;
	float speed;
	float time_per_move;
	float anim_framerate;
	bool paused = false;

};

