#pragma once

#include "common.hpp"
#include <deque>
#include <unordered_set>
#include <vector>

#include "IScene.hpp"

#include "EntityComponentSystem.hpp"
#include "ComponentManager.hpp"
#include "TextureManager.hpp"
#include "SnakeObject.hpp"
#include "SpriteAnim.hpp"
#include "assets.hpp"
#include "CollisionBox.hpp"


class SnakeGameScene : public IScene {
private:

public:
	SnakeGameScene() = default;
	~SnakeGameScene() = default;
	void initialise(World& world, TextureManager& texture_manager) override;
	void deinitialise(World& world, TextureManager& texture_manager) override;
	void update() override;

	//publics for SnakeObject
	enum class BoardTile {
		EMPTY,
		APPLE,
	};

	const static CollisionChannel::Channel snake_segment_collision_channel = CollisionChannel::c2;
	const static  CollisionChannel::Channel apple_particle_collision_channel = CollisionChannel::c3;
	std::tuple<float, float> boardPosToWorldPos(BoardPos pos);

	void collectApple();
	void createRetryScreen();
	void onDeath();


	std::unordered_set<u32> particles = {};
	BoardTile board[16][16] = { BoardTile::EMPTY };
private:
	void generateAppleParticles();
	void generateApplePos();
	void updateSnakes();

	World* world = nullptr;
	EntityComponentSystem* ecs = nullptr;
	TextureManager* texture_manager = nullptr;
	u32 BOARD_WIDTH = 16;
	u32 BOARD_HEIGHT = 16;
	std::vector<SnakeObject*> snakes;
	u32 apple_id = 0;
	BoardPos apple_pos = {0, 0};
	bool paused = false;
};

