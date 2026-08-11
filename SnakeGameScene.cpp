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
			8
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

//void SnakeGameScene::updateSnake() {
//	constexpr float snake_speed = 8;
//	constexpr float time_per_move = 1000. / snake_speed;
//	static float since_last_update = 0.;
//	since_last_update += TIME_STEP;
//
//	SnakeSegment& head = snake_segments.getComponent(snake.back());
//	if (next_dir == head.in_dir) {
//		next_dir = head.out_dir;
//	}
//	head.out_dir = next_dir;
//
//	// Look at direction enum explicit values for why this works
//	if (static_cast<i32>(head.out_dir) + static_cast<i32>(head.in_dir) == 0) {
//		// Straight
//		if (head.curr_type != SnakeSpriteType::STRAIGHT) {
//			ecs->getc<Sprite>(head.id).flip_flags = SpriteFlip::NONE;
//			ecs->getc<SpriteAnim>(head.id).current_frame += 8;
//			head.curr_type = SnakeSpriteType::STRAIGHT;
//		}
//	}
//	else {
//		//curved
//		// Set flipping for curved sprites (it runs left-to-up, clockwise)
//		Sprite& hv = ecs->getc<Sprite>(head.id);
//		switch (head.in_dir) {
//		case SnakeDir::LEFT:
//			hv.flip_flags = head.out_dir == SnakeDir::DOWN ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
//			break;
//		case SnakeDir::RIGHT:
//			hv.flip_flags = head.out_dir == SnakeDir::UP ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
//			break;
//		case SnakeDir::UP:
//			hv.flip_flags = head.out_dir == SnakeDir::LEFT ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
//			break;
//		case SnakeDir::DOWN:
//			hv.flip_flags = head.out_dir == SnakeDir::RIGHT ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
//			break;
//		}
//		if (head.curr_type != SnakeSpriteType::CURVED) {
//			ecs->getc<SpriteAnim>(head.id).current_frame -= 8;
//			head.curr_type = SnakeSpriteType::CURVED;
//		}
//	}
//	while (since_last_update > time_per_move) {
//		// Get tail, get head, place new head, remove tail if head moved into an empty space
//		SnakeSegment& head = snake_segments.getComponent(snake.back());
//		SnakeSegment& tail = snake_segments.getComponent(snake.front());
//		BoardPos new_pos = head.pos;
//		SnakeDir in_dir;
//		switch (next_dir) {
//		case SnakeDir::UP:
//			new_pos.y--;
//			in_dir = SnakeDir::DOWN;
//			break;
//		case SnakeDir::DOWN:
//			new_pos.y++;
//			in_dir = SnakeDir::UP;
//			break;
//		case SnakeDir::LEFT:
//			new_pos.x--;
//			in_dir = SnakeDir::RIGHT;
//			break;
//		case SnakeDir::RIGHT:
//			new_pos.x++;
//			in_dir = SnakeDir::LEFT;
//			break;
//		default:
//			new_pos.y--;
//			in_dir = SnakeDir::DOWN;
//			break;
//		}
//		// Bounds checking
//		if (new_pos.x < 0 || new_pos.x >= BOARD_WIDTH ||
//			new_pos.y < 0 || new_pos.y >= BOARD_HEIGHT)
//		{
//			onDeath();
//			return;
//		}
//
//		
//		if (board[new_pos.x][new_pos.y] != BoardTile::APPLE) {
//			destroySnakeSegment();
//		}
//		switch (board[new_pos.x][new_pos.y]) {
//		case BoardTile::EMPTY:
//		{	
//			// Set the new tail to play a reverse animation of the segment shrinking
//			u32 new_tail_id = snake.front();
//			SnakeSegment& new_tail = snake_segments.getComponent(new_tail_id);
//			SpriteAnim& new_tail_anim = ecs->getc<SpriteAnim>(new_tail_id);
//			new_tail_anim.anim_graph = { 0, 0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13, 14 };
//			new_tail_anim.since_last_update = 0;
//			Transform& new_tail_transform = ecs->getc<Transform>(new_tail_id);
//			Transform& new_tail_next = ecs->getNextc(new_tail_id);
//			Sprite& new_tail_view = ecs->getc<Sprite>(new_tail_id);
//			if (new_tail.curr_type == SnakeSpriteType::STRAIGHT) {
//				new_tail_transform.rot += 180;
//				new_tail_next.rot += 180;
//				new_tail_anim.current_frame = 15;
//			}
//			else {
//				new_tail_transform.rot += 90;
//				new_tail_next.rot += 90;
//				new_tail_anim.current_frame = 7;
//				if (new_tail_view.flip_flags == SpriteFlip::VERTICAL) {
//					new_tail_view.flip_flags = static_cast<SpriteFlip::SpriteFlip>(SpriteFlip::HORIZONTAL | SpriteFlip::VERTICAL);
//				}
//				else {
//					new_tail_view.flip_flags = SpriteFlip::VERTICAL;
//				}
//			}
//		}
//		break;
//		case BoardTile::APPLE:
//			generateAppleParticles();
//			generateApplePos();
//			break;
//		case BoardTile::SNAKE:
//			onDeath();
//			break;
//		}
//		// remove collision from old head
//		world->ecs.destroyComponent<CollisionBox>(snake.back());
//		u32 new_segment_id = createSnakeSegment({ 0, new_pos, in_dir, next_dir, SnakeSpriteType::STRAIGHT });
//		Transform& t = ecs->getc<Transform>(new_segment_id);
//		Transform& t_next = ecs->getNextc(new_segment_id);
//		switch (in_dir) {
//		case SnakeDir::DOWN:
//			t.rot += 90;
//			t_next.rot += 90;
//			[[fallthrough]];
//		case SnakeDir::RIGHT:
//			t.rot += 90;
//			t_next.rot += 90;
//			[[fallthrough]];
//		case SnakeDir::UP:
//			t.rot += 90;
//			t_next.rot += 90;
//		}
//		since_last_update -= time_per_move;
//	}
//}
//
//u32 SnakeGameScene::createSnakeSegment(SnakeSegment ss) {
//	const auto& [t_x, t_y] = boardPosToWorldPos(ss.pos);
//	u32 new_segment_id = ecs->createEntity(
//		Transform{ .pos = {t_x, t_y},
//								.scale = {2, 2}
//		},
//		Sprite{ .sprite_path = snake_sprite_path,
//								.sprite_id = texture_manager->getTextureid(snake_sprite_path),
//								.src_rect = {0, 0, 32, 32},
//								.zindex = 11
//		},
//		SpriteAnim{ .rows = 4, .cols = 4,
//								.cellw = 32, .cellh = 32,
//								.framerate = 60,
//								.anim_graph = { 1, 2, 3, 4, 5, 6, 7, 7, 9, 10, 11, 12, 13, 14, 15, 15 },
//								.current_frame = 8
//		},
//		CollisionBox{ .extent = {.x = 50, .y = 50},
//									.channels = static_cast<CollisionChannel::Channel>(CollisionChannel::c1 | SnakeGameScene::snake_segment_collision_channel),
//									.on_begin_collision = [this](u32 otherid)->void {
//										if (this->world->ecs.getc<CollisionBox>(otherid).channels & SnakeGameScene::apple_particle_collision_channel) {
//											this->world->ecs.destroyEntity(otherid);
//											this->particles.erase(particles.find(otherid));
//										}
//									}
//		}
//	);
//	ss.id = new_segment_id;
//	snake_segments.createComponent(new_segment_id, ss);
//	snake.emplace_back(new_segment_id);
//	board[ss.pos.x][ss.pos.y] = BoardTile::SNAKE;
//	return new_segment_id;
//}
//
//void SnakeGameScene::destroySnakeSegment() {
//	u32 id = snake.front();
//	ecs->destroyEntity(id);
//	BoardPos pos = snake_segments.getComponent(id).pos;
//	board[pos.x][pos.y] = BoardTile::EMPTY;
//	snake_segments.destroyComponent(id);
//	snake.pop_front();
//}

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