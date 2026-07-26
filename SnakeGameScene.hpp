#pragma once

#include "common.hpp"
#include <deque>

#include "IScene.hpp"

#include "EntityComponentSystem.hpp"
#include "ComponentManager.hpp"
#include "SpriteManager.hpp"
#include "SpriteAnim.hpp"
#include "assets.hpp"

class SnakeGameScene : public IScene {
public:
	SnakeGameScene() = default;

	void initialise(World& world, SpriteManager& sprite_manager) override;
	void deinitialise(World& world, SpriteManager& sprite_manager) override;
	void update() override;

	enum class SnakeDir {
		UP = 1,
		DOWN = -1,
		LEFT = 2,
		RIGHT = -2,
	};

	enum class BoardTile {
		EMPTY,
		APPLE,
		SNAKE
	};

	struct BoardPos {
		i32 x;
		i32 y;
	};

	enum class SnakeSpriteType {
		STRAIGHT,
		CURVED
	};

	struct SnakeSegment {
		u32 id;
		BoardPos pos;
		SnakeDir in_dir;
		SnakeDir out_dir;
		SnakeSpriteType curr_type;
	};

	SnakeDir next_dir = SnakeDir::RIGHT;

private:
	World* world = nullptr;
	EntityComponentSystem* ecs = nullptr;
	SpriteManager* sprite_manager = nullptr;

	void updateSnake();
	u32 createSnakeSegment(SnakeSegment ss);
	void destroySnakeSegment();
	void generateApplePos();
	void generateAppleParticles();
	void onDeath();
	void createRetryScreen();
	std::tuple<float, float> boardPosToWorldPos(BoardPos pos);
	
	u32 BOARD_WIDTH = 16;
	u32 BOARD_HEIGHT = 16;

	ComponentManager<SnakeSegment> snake_segments;
	BoardTile board[16][16] = { BoardTile::EMPTY };
	std::deque<u32> snake;
	u32 apple_id = 0;
	std::vector<u32> particles = {};
	bool paused = false;
};

