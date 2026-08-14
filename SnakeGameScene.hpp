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
	enum class BoardTileType {
		EMPTY,
		APPLE,
	};

	struct BoardTile {
		BoardTileType type = BoardTileType::EMPTY;
		u32 id = 0; // id of potential apple on tile
	};

	const static CollisionChannel::Channel snake_segment_collision_channel = CollisionChannel::c2;
	const static  CollisionChannel::Channel apple_particle_collision_channel = CollisionChannel::c3;
	std::tuple<float, float> boardPosToWorldPos(BoardPos pos);
	
	bool appleAt(BoardPos pos);
	void collectAppleAt(BoardPos pos);
	void collectParticle(u32 id);
	void createRetryScreen();
	void createNewAutoSnake();
	void onDeath();
	void setSnakeSpeeds(float new_speed);
	
	BoardTile board[16][16] = { };
	bool paused = false; // public for buy menu controller
	// publics for shop to set when upgrading (probably move to functions instead later)
	u32 apple_count = 1;
	u32 apple_particle_count = 20;
	u32 apple_particle_value = 1;
	u32 apple_value = 1;
	u32 current_score = 0;
	
private:
	void generateAppleParticles(u32 apple_id);
	SpriteColorMod getNewSnakeColor();
	void createNewApple();
	void updateSnakes();
	std::unordered_set<u32> particles = {};
	World* world = nullptr;
	EntityComponentSystem* ecs = nullptr;
	TextureManager* texture_manager = nullptr;
	u32 BOARD_WIDTH = 16;
	u32 BOARD_HEIGHT = 16;
	u32 new_snake_speed = 4;
	std::vector<SnakeObject*> snakes;
	std::unordered_set<u32> apple_ids;
};

