#include "SnakeGameScene.hpp"

#include <random>

#include "common.hpp"
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
#include "RetryMenuController.hpp"
#include "Timer.hpp"

void SnakeGameScene::initialise(World& world, TextureManager& texture_manager) {
	this->world = &(world);
	this->ecs = &(world.ecs);
	this->texture_manager = &(world.texture_manager);
	// sprites init
	texture_manager.loadTexture(snake_sprite_path);
	texture_manager.loadTexture(apple_sprite_path);
	texture_manager.loadTexture(board_sprite_path);
	texture_manager.loadTexture(pause_sprite_path);

	// board init
	ecs->createEntity(
		Transform{ .scale = {31.25, 31.25}
		},
		Sprite{ .sprite_path = board_sprite_path,
								.sprite_id = texture_manager.getTextureid(board_sprite_path),
								.src_rect = { 0, 0, texture_manager.getTexture(board_sprite_path).w, texture_manager.getTexture(board_sprite_path).h },
								.zindex = 9
		}
	);

	snakes.push_back(
		new SnakeObject(
			world,
			(*this),
			16
		)
	);

	// apple init
	apple_id = ecs->createEntity(
		Transform{ },
		Sprite{ .sprite_path = apple_sprite_path,
								.sprite_id = texture_manager.getTextureid(apple_sprite_path),
								.src_rect = { 0, 0, 64, 64 },
								.zindex = 11
		}
	);
	generateApplePos();
	IUserController* snake_controller = new SnakeController(*snakes.back());

	IUserController* pause_controller = new PauseController(*this, world);

	InputSystem::instance().pushContext();
	InputSystem::instance().addToCurrentContext(snake_controller);
	InputSystem::instance().addToCurrentContext(pause_controller);
};

void SnakeGameScene::deinitialise(World& world, TextureManager& texture_manager) {
	texture_manager.unloadTexture(snake_sprite_path);
	texture_manager.unloadTexture(apple_sprite_path);
	texture_manager.unloadTexture(board_sprite_path);
	texture_manager.unloadTexture(pause_sprite_path);
	for (SnakeObject* s : snakes) {
		delete s;
	}
	snakes.clear();
	for (u32 particle : particles) {
		ecs->destroyEntity(particle);
	}
	ecs->destroyEntity(apple_id);
	InputSystem::instance().clearContexts();
	world.timer_manager.clearTimers();
	world.timeline_manager.clearTimelines();
}

void SnakeGameScene::update() {
	if (!paused) {
		updateSnakes();
	}
}

void SnakeGameScene::updateSnakes() {
	for (SnakeObject* snake : snakes) {
		snake->updateSnake();
	}
}

void SnakeGameScene::collectApple() {
	board[apple_pos.x][apple_pos.y] = BoardTile::EMPTY;
	generateAppleParticles();
	generateApplePos();
}

void SnakeGameScene::generateApplePos() {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<u32> x_range{ 0, BOARD_WIDTH - 1 };
	std::uniform_int_distribution<u32> y_range{ 0, BOARD_HEIGHT - 1 };
	bool placed = false;
	i32 x, y;
	while (!placed) {
		x = x_range(mt);
		y = y_range(mt);
		if (board[x][y] == BoardTile::EMPTY) {
			board[x][y] = BoardTile::APPLE;
			placed = true;
		}
		else {
			continue;
		}
	}

	const auto& [new_x, new_y] = boardPosToWorldPos({ x, y });
	ecs->getc<Transform>(apple_id).pos = { new_x, new_y };
	ecs->getNextc(apple_id).pos = { new_x, new_y };
	apple_pos.x = x;
	apple_pos.y = y;
}

void SnakeGameScene::generateAppleParticles() {
	const Transform apple_transform = ecs->getc<Transform>(apple_id);
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<i32> view_range{ 0, 56 };
	std::uniform_real_distribution<float> vel_range{ -10., 10. };
	std::uniform_real_distribution<float> scale_range{ 1., 3. };
	std::uniform_real_distribution<float> rot_range{ 0, 30 };
	std::vector<u32> apple_ids = {};
	for (i32 i = 0; i < 10; ++i) {
		u32 new_particle_id = ecs->createEntity(
			Transform{ .pos = {apple_transform.pos.x, apple_transform.pos.y},
									.scale = {scale_range(mt), scale_range(mt)}
			},
			Sprite{ .sprite_path = apple_sprite_path,
							.sprite_id = texture_manager->getTextureid(apple_sprite_path),
							.src_rect = {view_range(mt), view_range(mt), 8, 8}
			},
			LinearVelocity{ .velocity = {vel_range(mt), vel_range(mt)},
											.friction = 0.1f
			},
			AngularVelocity{ .velocity = rot_range(mt)
			}
		);
		particles.emplace(new_particle_id);
		apple_ids.push_back(new_particle_id);
	}
	world->timer_manager.createTimer(
		Timer{	.interval = 500,
						.callback = [this, apple_ids]()->void {
							for (u32 id : apple_ids) {
								this->world->ecs.createComponent(id, CollisionBox{ .extent = {.x = 15, .y = 15}, .channels = static_cast<CollisionChannel::Channel>(CollisionChannel::c1 | SnakeGameScene::apple_particle_collision_channel) });
							}
						},
		}
	);
}

std::tuple<float, float> SnakeGameScene::boardPosToWorldPos(BoardPos pos) {
	static float tile_width = INTERNAL_WINDOW_WIDTH / static_cast<float>(BOARD_WIDTH);
	static float tile_height = INTERNAL_WINDOW_HEIGHT / static_cast<float>(BOARD_HEIGHT);
	float x = static_cast<float>(pos.x) * tile_width - INTERNAL_WINDOW_WIDTH / 2.f + (tile_width / 2.f);
	float y = static_cast<float>(pos.y) * tile_width - INTERNAL_WINDOW_HEIGHT / 2.f + (tile_height / 2.f);
	return { x, -y };
};

void SnakeGameScene::onDeath() {
	paused = true;
	// play death animation
	// show retry screen
	createRetryScreen();
}

void SnakeGameScene::createRetryScreen() {
	texture_manager->loadTexture(game_over_sprite_path);
	texture_manager->loadTexture(retry_sprite_path);
	texture_manager->loadTexture(menu_sprite_path);
	texture_manager->loadTexture(quit_sprite_path);
	texture_manager->loadTexture(arrow_sprite_path);
	// retry logo
	u32 retry_logo_id = ecs->createEntity(
		Transform{	.pos {.x = 0, .y = 100}, 
								.scale = {.x = 4, .y = 4}
		},
		Sprite{ .sprite_path = game_over_sprite_path,
								.sprite_id = texture_manager->getTextureid(game_over_sprite_path),
								.src_rect = {0, 0, texture_manager->getTexture(game_over_sprite_path).w, texture_manager->getTexture(game_over_sprite_path).h },
								.zindex = 12
		}
	);
	// retry option
	u32 retry_option_id = ecs->createEntity(
		Transform{	.pos {.x = -300, .y = -150},
								.scale = {.x = 2, .y = 2}
		},
		Sprite{ .sprite_path = retry_sprite_path,
								.sprite_id = texture_manager->getTextureid(retry_sprite_path),
								.src_rect = {0, 0, texture_manager->getTexture(retry_sprite_path).w, texture_manager->getTexture(retry_sprite_path).h },
								.zindex = 12
		}
	);
	// main menu option
	u32 retry_menu_id = ecs->createEntity(
		Transform{	.pos {.x = 0, .y = -150},
								.scale = {.x = 2, .y = 2}

		},
		Sprite{ .sprite_path = menu_sprite_path,
								.sprite_id = texture_manager->getTextureid(menu_sprite_path),
								.src_rect = {0, 0, texture_manager->getTexture(menu_sprite_path).w, texture_manager->getTexture(menu_sprite_path).h },
								.zindex = 12
		}
	);
	// quit option
	u32 retry_quit_id = ecs->createEntity(
		Transform{	.pos {.x = 300, .y = -150}, 
								.scale = {.x = 2, .y = 2}
		},
		Sprite{ .sprite_path = quit_sprite_path,
								.sprite_id = texture_manager->getTextureid(quit_sprite_path),
								.src_rect = {0, 0, texture_manager->getTexture(quit_sprite_path).w, texture_manager->getTexture(quit_sprite_path).h },
								.zindex = 12
		}
	);
	// selection arrow
	u32 arrow_id = ecs->createEntity(
		Transform{ .pos {.x = -450, .y = -150},
								.scale = {.x = 2, .y = 2}
		},
		Sprite{ .sprite_path = arrow_sprite_path,
								.sprite_id = texture_manager->getTextureid(arrow_sprite_path),
								.src_rect = {0, 0, texture_manager->getTexture(arrow_sprite_path).w, texture_manager->getTexture(arrow_sprite_path).h },
								.zindex = 12
		}
	);

	// retry screen controller
	IUserController* retry_controller = new RetryMenuController(
		*world,
		retry_logo_id,
		retry_option_id,
		retry_menu_id,
		retry_quit_id,
		arrow_id
	);
	InputSystem::instance().pushContext();
	InputSystem::instance().addToCurrentContext(retry_controller);
}