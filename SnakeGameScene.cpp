#include "SnakeGameScene.hpp"

#include <random>

#include "AISnakeController.hpp"
#include "common.hpp"
#include "BuyController.hpp"
#include "BuyScreenObject.hpp"
#include "Sprite.hpp"
#include "SpriteAnim.hpp"
#include "LinearVelocity.hpp"
#include "AngularVelocity.hpp"
#include "CollisionBox.hpp"
#include "World.hpp"
#include "InputSystem.hpp"
#include "IUserController.hpp"
#include "SnakeController.hpp"
#include "SnakeObject.hpp"
#include "PauseController.hpp"
#include "Timer.hpp"
#include "util.hpp"

void SnakeGameScene::initialise(World& world, TextureManager& texture_manager) {
	this->world = &(world);
	this->ecs = &(world.ecs);
	this->texture_manager = &(world.texture_manager);
	// sprites init
	texture_manager.loadTexture(snake_sprite_path);
	texture_manager.loadTexture(apple_sprite_path);
	texture_manager.loadTexture(board_sprite_path);
	// for pause screen
	texture_manager.loadTexture(pause_sprite_path);
	texture_manager.loadTexture(resume_sprite_path);
	texture_manager.loadTexture(menu_sprite_path);
	texture_manager.loadTexture(quit_sprite_path);
	texture_manager.loadTexture(arrow_sprite_path);

	// board init
	ecs->createEntity(
		Transform{ .scale = {31.25, 31.25} },
		Sprite{ 
			.sprite_path = board_sprite_path,
			.texture_id = texture_manager.getTextureid(board_sprite_path),
			.src_rect = { 0, 0, texture_manager.getTexture(board_sprite_path).w, texture_manager.getTexture(board_sprite_path).h },
			.zindex = 9
		}
	);
	// Player snake
	snakes.push_back(
		new SnakeObject(
			world,
			(*this),
			new_snake_speed,
			SpriteColorMod{.r = 255, .g = 255, .b = 255}
		)
	);

	// Controllers setup
	IUserController* snake_controller = new SnakeController(*snakes.back());
	IUserController* pause_controller = new PauseController(*this, world);
	InputSystem::instance().pushContext();
	InputSystem::instance().addToCurrentContext(snake_controller);
	InputSystem::instance().addToCurrentContext(pause_controller);

	for (i32 i = 0; i < apple_count; ++i) {
		createNewApple();
	}
	for (i32 i = 0; i < auto_snake_count; ++i) {
		createNewAutoSnake();
	}

	buy_screen = new BuyScreenObject(*this, world, { .x = 0, .y = -1000 }, { .x = 0, .y = 0 });
};

void SnakeGameScene::deinitialise(World& world, TextureManager& texture_manager) {
	texture_manager.unloadTexture(snake_sprite_path);
	texture_manager.unloadTexture(apple_sprite_path);
	texture_manager.unloadTexture(board_sprite_path);
	
	//for pause elements
	world.texture_manager.unloadTexture(game_over_sprite_path);
	world.texture_manager.unloadTexture(resume_sprite_path);
	world.texture_manager.unloadTexture(menu_sprite_path);
	world.texture_manager.unloadTexture(quit_sprite_path);
	world.texture_manager.unloadTexture(arrow_sprite_path);
	
	for (SnakeObject* s : snakes) {
		delete s;
	}
	snakes.clear();
	for (auto particle : particles) {
		ecs->destroyEntity(particle.first);
		world.timer_manager.destroyTimer(particle.second);
	}
	for (u32 apple_id : apple_ids) {
		ecs->destroyEntity(apple_id);
	}
	InputSystem::instance().clearContexts();
	world.timer_manager.clearTimers();
	world.timeline_manager.clearTimelines();
	for (IAIController* ai_controller : world.ai_controllers) {
		delete ai_controller;
	}
	world.ai_controllers.clear();
	delete buy_screen;
	buy_screen = nullptr;
	this->world = nullptr;
	this->texture_manager = nullptr;
}

void SnakeGameScene::update() {
	if (!paused) {
		updateSnakes();
	}
}

bool SnakeGameScene::appleAt(BoardObject::BoardPos pos) {
	return board.board[pos.x][pos.y] == BoardObject::BoardTile::APPLE;
}

void SnakeGameScene::collectAppleAt(BoardObject::BoardPos pos) {
	board.board[pos.x][pos.y] = BoardObject::BoardTile::EMPTY;
	u32 apple_id = apple_board[pos.x][pos.y];
	generateAppleParticles(apple_id);
	world->ecs.destroyEntity(apple_id);
	apple_ids.erase(apple_ids.find(apple_id));
	createNewApple();
	current_score += apple_value;
}

void SnakeGameScene::collectParticle(u32 particle_id, u32 snake_id) {
	ecs->destroyComponent<CollisionBox>(particle_id);
	Transform snake_head_t = world->ecs.getc<Transform>(snake_id);
	Transform particle_start_t = world->ecs.getc<Transform>(particle_id);
	world->timeline_manager.createTimeline(
		Timeline{
			.length = 550,
			.on_update = [=](float delta)->void {
				Transform& particle_t = world->ecs.getNextc(particle_id);
				Transform new_t = particle_t;
				if (world->ecs.exists<Transform>(snake_id)) {
					new_t = lerp(particle_start_t, world->ecs.getNextc(snake_id), delta);
				}
				particle_t.pos = new_t.pos;
			},
			.on_end = [=]()->void {
				ecs->destroyEntity(particle_id);
				this->world->timer_manager.destroyTimer(this->particles[particle_id]);
				this->particles.erase(particle_id);
				this->current_score += apple_particle_value;					
			},
			.delta_curve = [](float delta)->float {
				return (-2 + std::pow((1 / (delta + 0.5) - 1), 2) + std::pow(2.9, delta));
				return delta;
			}		
		}
	);
}

void SnakeGameScene::createNewApple() {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<i32> x_range{ 0, BOARD_WIDTH - 1 };
	std::uniform_int_distribution<i32> y_range{ 0, BOARD_HEIGHT - 1 };
	bool placed = false;
	i32 x, y;
	while (!placed) {
		x = x_range(mt);
		y = y_range(mt);
		if (board.board[x][y] == BoardObject::BoardTile::EMPTY) {
			board.board[x][y] = BoardObject::BoardTile::APPLE;
			placed = true;
		}
		else {
			continue;
		}
	}

	vec2 new_pos = board.boardPosToWorldPos({x, y});
	u32 new_apple_id = ecs->createEntity(
		Transform{ .pos = { new_pos.x, new_pos.y} },
		Sprite{ 
			.sprite_path = apple_sprite_path,
			.texture_id = world->texture_manager.getTextureid(apple_sprite_path),
			.src_rect = { 0, 0, 64, 64 },
			.zindex = 11
		}
	); 
	apple_ids.emplace(new_apple_id);
	apple_board[x][y] = new_apple_id;
}

void SnakeGameScene::createNewAutoSnake() {
	SnakeObject* new_snake = new SnakeObject(
		(*world),
		(*this),
		new_snake_speed,
		getNewSnakeColor()
	);
	snakes.push_back(new_snake);
	world->ai_controllers.push_back(new AISnakeController((*new_snake), (*this)));
}

void SnakeGameScene::generateAppleParticles(u32 apple_id) {
	const Transform apple_transform = ecs->getc<Transform>(apple_id);
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<i32> view_range{ 0, 56 };
	std::uniform_real_distribution<float> vel_range{ -10., 10. };
	std::uniform_real_distribution<float> vel_mag_range{ 3.5, 8.5 };
	std::uniform_real_distribution<float> scale_range{ 1., 3. };
	std::uniform_real_distribution<float> rot_range{ 0, 30 };
	std::vector<u32> apple_ids = {};
	for (i32 i = 0; i < apple_particle_count; ++i) {
		vec2 vel_vec = { vel_range(mt), vel_range(mt) };
		vel_vec.normalise();
		vel_vec = vel_vec * vel_mag_range(mt);
		u32 new_particle_id = ecs->createEntity(
			Transform{ 
				.pos = {apple_transform.pos.x, apple_transform.pos.y},
				.scale = {scale_range(mt), scale_range(mt)}
			},
			Sprite{ 
				.sprite_path = apple_sprite_path,
				.texture_id = texture_manager->getTextureid(apple_sprite_path),
				.src_rect = {view_range(mt), view_range(mt), 8, 8}
			},
			LinearVelocity{ 
				.velocity = vel_vec,
				.friction = 0.1f
			},
			AngularVelocity{ 
				.velocity = rot_range(mt)
			}
		);
	u32 particle_cleanup_timer_id = world->timer_manager.createTimer(
		Timer{
			.interval = 120000,
			.callback = [=]()->void {
				this->ecs->destroyEntity(new_particle_id);
				//this->world->timer_manager.destroyTimer(this->particles[new_particle_id]);
				this->particles.erase(new_particle_id);
			},
		}
	);

		particles.emplace(new_particle_id, particle_cleanup_timer_id);
		apple_ids.push_back(new_particle_id);
	}
	world->timer_manager.createTimer(
		Timer{	
			.interval = 500,
			.callback = [this, apple_ids]()->void {
				for (u32 id : apple_ids) {
					this->world->ecs.createComponent(id, CollisionBox{ .extent = {.x = 15, .y = 15}, .channels = static_cast<CollisionChannel::Channel>(CollisionChannel::c1 | SnakeGameScene::apple_particle_collision_channel) });
				}
			},
		}
	);

}

SpriteColorMod SnakeGameScene::getNewSnakeColor() {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<i32> color_range{ 0, 360 };
	const auto& [r, g, b] = hsvTorgb(color_range(mt), 0.9f, 0.9f);
	return SpriteColorMod{ .r = r, .g = g, .b = b };
}

void SnakeGameScene::setSnakeSpeeds(float new_speed) {
	for (SnakeObject* snake : snakes) {
		snake->setSpeed(new_speed);
	}
}

void SnakeGameScene::updateSnakes() {
	for (SnakeObject* snake : snakes) {
		snake->updateSnake();
	}
}
