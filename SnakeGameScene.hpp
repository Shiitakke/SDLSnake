#pragma once

#include "common.hpp"
#include <deque>
#include <unordered_set>
#include <vector>

#include "IScene.hpp"

#include "EntityComponentSystem.hpp"
#include "BitmapFont.hpp"
#include "BoardObject.hpp"
#include "ComponentManager.hpp"
#include "TextureManager.hpp"
#include "SnakeObject.hpp"
#include "SpriteAnim.hpp"
#include "assets.hpp"
#include "CollisionBox.hpp"
#include "vec.hpp"

class BuyScreenObject;

class SnakeGameScene : public IScene {
public:
	SnakeGameScene() = default;
	~SnakeGameScene() = default;
	void initialise(World& world, TextureManager& texture_manager) override;
	void deinitialise(World& world, TextureManager& texture_manager) override;
	void update() override;

	const static CollisionChannel::Channel snake_segment_collision_channel = CollisionChannel::c2;
	const static  CollisionChannel::Channel apple_particle_collision_channel = CollisionChannel::c3;
	
	bool appleAt(BoardObject::BoardPos pos);
	void collectAppleAt(BoardObject::BoardPos pos);
	void collectParticle(u32 id, u32 snake_id, SnakeObject* snake);
	//void createRetryScreen();
	void createNewAutoSnake();
	void setSnakeSpeeds(float new_speed);
	
	BoardObject board;
	bool paused = false; // public for buy menu controller
	// publics for shop to set when upgrading (probably move to functions instead later)
	u32 apple_count = 10;
	u32 apple_particle_count = 10;
	u32 apple_particle_value = 1;
	u32 apple_value = 1;
	u32 current_score = 0;
	u32 respawn_timer = 1000;
	u32 auto_snake_count = 4;
	u32 new_snake_speed = 8;

private:
	void createNewApple();
	void generateAppleParticles(u32 apple_id);
	SpriteColorMod getNewSnakeColor();
	void updateSnakes();

	//std::unordered_set<u32> particles = {};
	std::unordered_map<u32, u32> particles = {};
	BitmapFont testfont;
	World* world = nullptr;
	EntityComponentSystem* ecs = nullptr;
	TextureManager* texture_manager = nullptr;
	BuyScreenObject* buy_screen = nullptr;
	i32 BOARD_WIDTH = 16;
	i32 BOARD_HEIGHT = 16;
	std::vector<SnakeObject*> snakes;
	u32 apple_board[16][16];
	std::unordered_set<u32> apple_ids;
};

