#pragma once

#include <deque>

#include "BitmapFont.hpp"
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
	
	BoardObject::BoardPos getHeadPos();
	std::tuple<SnakeObject::SnakeDir, BoardObject::BoardPos> getNextPos();
	void init();
	void killSnake();
	void setSpeed(float new_speed);
	void setDirection(SnakeDir new_direction);
	void updateSnake();
	void updateCollectionIndicator(
		u32 apple_particle_value,
		BitmapFont testfont
	);

	BoardObject board; // public for ai snake controller
	u32 running_collection_total = 0;
	u32 running_collection_total_reset_timer_id = 0;
	BitmapFontWord running_collection_indicator;
	u32 running_collection_indicator_timer_id;
	BitmapFontWord running_collection_indicator2;
	u32 running_collection_indicator_timer_id2;
	bool running_collection_indicator_exists = false;
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

	void createHeadCollisionTimeline();
	u32 createSnakeSegment(SnakeSegment ss);
	void destroySnakeSegment();
	void reverseTailAnimation();

	World& world;
	SnakeGameScene& scene;
	// Every snake has its own board to track itself, as they cant collide with each other
	// Snakes check in with the appleboard to see if theres an apple wherever they are moving
	std::deque<SnakeSegment> segments = {};
	vec2 prev_head_collision_pos;
	vec2 next_head_collision_pos;
	u32 head_collision_id;
	u32 head_collision_timeline_id;
	SnakeDir next_dir = SnakeDir::RIGHT;
	float since_last_update = 0.0f;
	float speed;
	float time_per_move;
	float anim_framerate;
	bool paused = false;
	SpriteColorMod snake_color;
};

