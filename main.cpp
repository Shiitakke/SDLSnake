#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>

#include <stdio.h>
#include <deque>
#include <random>
#include <ctime>
#include <vector>
#include <deque>
#include <unordered_map>

#include "common.hpp"
#include "util.hpp"

#include "IdManager.hpp"
#include "SpriteManager.hpp"

#include "Sprite.hpp"
#include "SpriteAnim.hpp"
#include "SpriteView.hpp"
#include "Transform.hpp"
#include "ComponentManager.hpp"
#include "LinearVelocity.hpp"
#include "AngularVelocity.hpp"
#include "Camera.hpp"

#include "RenderingSystem.hpp"
#include "EntityComponentSystem.hpp"

#include <iostream>

namespace snakedir {
	enum direction {
		UP = 1,
		DOWN = -1,
		LEFT = 2,
		RIGHT = -2,
	};
}

namespace tile {
	enum tile {
		EMPTY,
		APPLE,
		SNAKE
	};
}

struct BoardPos {
	i32 x;
	i32 y;
};

enum spriteType {
	STRAIGHT,
	CURVED
};

struct SnakeSegment {
	u32 id;
	BoardPos pos;
	snakedir::direction in_dir;
	snakedir::direction out_dir;
	spriteType curr_type;
};

bool init();
bool initSdl();
void initGame();
void close();

u32 getNextId();
void changePause();
void generateApplePos();
void generateAppleParticles();
std::tuple<float, float> boardPosToWorldPos(BoardPos pos);
void updateSnake(snakedir::direction& d);
void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<SpriteView>& views
);
void updateLinearVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<LinearVelocity>& linear_velocities
);
void updateAngularVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<AngularVelocity>& angular_velocities
);
u32 createSnakeSegment(SnakeSegment ss);
void destroySnakeSegment();

// Engine stuff
bool quit = false;
static IdManager id_manager;
static SpriteManager sprite_manager;
static RenderingSystem* rendering_system;
static EntityComponentSystem ecs;

// Gameplay stuff
constexpr u32 BOARD_WIDTH = 16;
constexpr u32 BOARD_HEIGHT = 16;
std::string snake_sprite_path = "./assets/snake.png";
std::string apple_sprite_path = "./assets/apple.png";
std::string board_sprite_path = "./assets/board.png";
std::string pause_sprite_path = "./assets/pause.png";
static ComponentManager<SnakeSegment> snake_segments;
std::deque<u32> snake;
u32 apple_id = getNextId();

tile::tile board[BOARD_WIDTH][BOARD_HEIGHT] = { tile::EMPTY };

int main(int argc, char* args[]) {
	if (!init()) {
		return 1;
	}

	snakedir::direction next_dir = snakedir::RIGHT;
	//Sdl loop init
	SDL_Event e;
	u64 start = SDL_GetTicks64();
	u64 end = start;
	float period = 0;
	bool paused = true;
	
	while (quit == false) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
				rendering_system->handleWindowResize();
			}
			if (e.type == SDL_KEYDOWN) {
				if (paused) {
					paused = false;
					changePause();
				}
				switch (e.key.keysym.sym) {
					case SDLK_UP:
						next_dir = snakedir::UP;
						break;
					case SDLK_DOWN:
						next_dir = snakedir::DOWN;
						break;
					case SDLK_LEFT:
						next_dir = snakedir::LEFT;
						break;
					case SDLK_RIGHT:
						next_dir = snakedir::RIGHT;
						break;
					case SDLK_ESCAPE:
						paused = true;
						changePause();
						break;
				}
			}
		}
		
		while (period > TIME_STEP) {
			ComponentManager<Transform>& transforms = ecs.getComponentManager<Transform>();
			transforms.components = transforms.next_transforms;

			updateSnake(next_dir);
			updateAnims(
				ecs.getComponentManager<SpriteAnim>(),
				ecs.getComponentManager<SpriteView>()
			);
		
			updateLinearVelocities(
				ecs.getComponentManager<Transform>(),
				ecs.getComponentManager<LinearVelocity>()
			);
			updateAngularVelocities(
				ecs.getComponentManager<Transform>(),
				ecs.getComponentManager<AngularVelocity>()
			);
			
			period -= TIME_STEP;
		}
		rendering_system->render(period / TIME_STEP);
		end = SDL_GetTicks64();
		period += (end - start) * GAME_SPEED_SCALAR * !paused;
		start = end;
	}
	close();
	return 0;
}

u32 getNextId() {
	return ecs.getNextId();
}

void generateApplePos() {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<u32> x_range{0, BOARD_WIDTH - 1};
	std::uniform_int_distribution<u32> y_range{0, BOARD_HEIGHT - 1};
	bool placed = false;
	i32 x, y;
	while (!placed) {
		x = x_range(mt);
		y = y_range(mt);
		if (board[x][y] == tile::EMPTY) {
			board[x][y] = tile::APPLE;
			placed = true;
		} else {
			continue;
		}
	}

	std::tuple<float, float> new_apple_pos = boardPosToWorldPos({x, y});
	ecs.getc<Transform>(apple_id).pos = {std::get<0>(new_apple_pos), std::get<1>(new_apple_pos)};
	ecs.getNextc(apple_id).pos = { std::get<0>(new_apple_pos), std::get<1>(new_apple_pos)};
}

void generateAppleParticles() {
	const Transform apple_transform = ecs.getc<Transform>(apple_id);
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<i32> view_range{ 0, 56 };
	std::uniform_real_distribution<float> vel_range{ -10., 10. };
	std::uniform_real_distribution<float> scale_range{ 1., 3. };
	std::uniform_real_distribution<float> rot_range{ 0, 30 };
	for (i32 i = 0; i < 10; ++i) {
		u32 nextid = getNextId();
		ecs.createEntity(nextid,
			Transform	{ .id = nextid, 
									.pos = {apple_transform.pos.x, apple_transform.pos.y}, 
									.scale = {scale_range(mt), scale_range(mt)} 
								},
			SpriteView{ .id = nextid, 
									.sprite_path = apple_sprite_path, 
									.src_rect = {view_range(mt), view_range(mt), 8, 8} 
								},
			LinearVelocity{ .id = nextid, 
											.velocity = {vel_range(mt), vel_range(mt)}, 
											.friction = 0.1f 
										},
			AngularVelocity { .id = nextid, 
												.velocity = rot_range(mt) 
											}
		);
	}
}

std::tuple<float, float> boardPosToWorldPos(BoardPos pos) {
	static constexpr float tile_width = INTERNAL_WINDOW_WIDTH / static_cast<float>(BOARD_WIDTH);
	static constexpr float tile_height = INTERNAL_WINDOW_HEIGHT / static_cast<float>(BOARD_HEIGHT);
	float x = static_cast<float>(pos.x) * tile_width - INTERNAL_WINDOW_WIDTH / 2.f + (tile_width / 2.f);
	float y = static_cast<float>(pos.y) * tile_width - INTERNAL_WINDOW_HEIGHT / 2.f + (tile_height / 2.f);
	return {x, -y};
}

void updateSnake(snakedir::direction& d) {
	constexpr float snake_speed = 8;
	constexpr float time_per_move = 1000./snake_speed;
	static float since_last_update = 0.;
	since_last_update += TIME_STEP;
	
	SnakeSegment& head = snake_segments.getComponent(snake.back());
	if (d == head.in_dir) {
		d = head.out_dir;
	}
	head.out_dir = d;

	// Look at direction enum explicit values for why this works
	if (head.out_dir + head.in_dir == 0) {
		// Straight
		if (head.curr_type != STRAIGHT) {
			ecs.getc<SpriteView>(head.id).flip_flags = SDL_FLIP_NONE;
			ecs.getc<SpriteAnim>(head.id).current_frame += 8;
			head.curr_type = STRAIGHT;
		}
	}
	else {
		//curved
		// Set flipping for curved sprites (it runs left-to-up, clockwise)
		SpriteView& hv = ecs.getc<SpriteView>(head.id);
		switch (head.in_dir) {
		case snakedir::LEFT:
			hv.flip_flags = head.out_dir == snakedir::DOWN ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			break;
		case snakedir::RIGHT:
			hv.flip_flags = head.out_dir == snakedir::UP ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			break;
		case snakedir::UP:
			hv.flip_flags = head.out_dir == snakedir::LEFT ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			break;
		case snakedir::DOWN:
			hv.flip_flags = head.out_dir == snakedir::RIGHT ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			break;
		}
		if (head.curr_type != CURVED) {
			ecs.getc<SpriteAnim>(head.id).current_frame -= 8;
			head.curr_type = CURVED;
		}
	}


	while (since_last_update > time_per_move) {
		// Get tail, get head, place new head, remove tail if head moved into an empty space
		SnakeSegment& head = snake_segments.getComponent(snake.back());
		SnakeSegment& tail = snake_segments.getComponent(snake.front());
		BoardPos new_pos = head.pos;
		snakedir::direction in_dir;
		switch (d) {
			case snakedir::UP:
				new_pos.y--;
				in_dir = snakedir::DOWN;
				break;
			case snakedir::DOWN:
				new_pos.y++;
				in_dir = snakedir::UP;
				break;
			case snakedir::LEFT:
				new_pos.x--;
				in_dir = snakedir::RIGHT;
				break;
			case snakedir::RIGHT:
				new_pos.x++;
				in_dir = snakedir::LEFT;
				break;
			default:
				new_pos.y--;
				in_dir = snakedir::DOWN;
				break;
		}
		// Bounds checking
		if (new_pos.x < 0 || new_pos.x >= BOARD_WIDTH ||
				new_pos.y < 0 || new_pos.y >= BOARD_HEIGHT) 
		{
			quit = true;
			return;
		}

		switch (board[new_pos.x][new_pos.y]) {
			case tile::EMPTY:
			{
				destroySnakeSegment();
				// Set the new tail to play a reverse animation of the segment shrinking
				u32 new_tail_id = snake.front();
				SnakeSegment& new_tail = snake_segments.getComponent(new_tail_id);
				SpriteAnim& new_tail_anim = ecs.getc<SpriteAnim>(new_tail_id);
				new_tail_anim.anim_graph = {0, 0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13, 14};
				Transform& new_tail_transform = ecs.getc<Transform>(new_tail_id);
				Transform& new_tail_next = ecs.getNextc(new_tail_id);
				SpriteView& new_tail_view = ecs.getc<SpriteView>(new_tail_id);
				if (new_tail.curr_type == STRAIGHT) {
					new_tail_transform.rot += 180;
					new_tail_next.rot += 180;
					new_tail_anim.current_frame = 15;
				} else {
					new_tail_transform.rot += 90;
					new_tail_next.rot += 90;
					new_tail_anim.current_frame = 7;
					if (new_tail_view.flip_flags == SDL_FLIP_VERTICAL) {
						new_tail_view.flip_flags = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
					} else {
						new_tail_view.flip_flags = SDL_FLIP_VERTICAL;
					}
				}
			}
				break;
			case tile::APPLE:
				generateAppleParticles();
				generateApplePos();
				break;
			case tile::SNAKE:
				quit = true;
				break;
		}
		u32 new_segment_id = createSnakeSegment({0, new_pos, in_dir, d, STRAIGHT});
		Transform& t = ecs.getc<Transform>(new_segment_id);
		Transform& t_next = ecs.getNextc(new_segment_id);
		switch (in_dir) {
			case snakedir::DOWN:
				t.rot += 90;
				t_next.rot += 90;
			case snakedir::RIGHT:
				t.rot += 90;
				t_next.rot += 90;
			case snakedir::UP:
				t.rot += 90;
				t_next.rot += 90;
		}
		since_last_update -= time_per_move;
	}
}

u32 createSnakeSegment(SnakeSegment ss) {
	u32 new_segment_id = getNextId();
	ss.id = new_segment_id;
	std::tuple<float, float> t_pos = boardPosToWorldPos(ss.pos);
	ecs.createEntity( new_segment_id,
		Transform	{	.id = new_segment_id, 
								.pos = {std::get<0>(t_pos), std::get<1>(t_pos)}, 
								.scale = {2, 2} 
							},
		SpriteView{ .id = new_segment_id, 
								.sprite_path = snake_sprite_path, 
								.src_rect = {0, 0, 32, 32}, 
								.zindex = 11 
							},
		SpriteAnim{ .id = new_segment_id, 
								.rows = 4, .cols = 4, 
								.cellw = 32, .cellh = 32, 
								.framerate = 56, 
								.anim_graph = { 1, 2, 3, 4, 5, 6, 7, 7, 9, 10, 11, 12, 13, 14, 15, 15 }, 
								.current_frame = 8
							}

	);
	snake_segments.createComponent(new_segment_id, ss);
	snake.emplace_back(new_segment_id);
	board[ss.pos.x][ss.pos.y] = tile::SNAKE;
	return new_segment_id;
}

void destroySnakeSegment() {
	u32 id = snake.front();
	ecs.destroyEntity(id);
	BoardPos pos = snake_segments.getComponent(id).pos;
	board[pos.x][pos.y] = tile::EMPTY;
	snake_segments.destroyComponent(id);
	snake.pop_front();
}

void changePause() {
	// pause screen image init
	static u32 pauseid = getNextId();
	static bool is_paused = false;
	if (!is_paused) {
		Sprite& pause_sprite = sprite_manager.getSprite(pause_sprite_path);
		ecs.createEntity(pauseid, 
			Transform	{ .id = pauseid,
									.scale = {4, 4}
								},
			SpriteView{ .id = pauseid,
									.sprite_path = pause_sprite_path,
									.src_rect = {0, 0, pause_sprite.w, pause_sprite.h},
									.zindex = 15
								}
		);
	} else {
		ecs.destroyEntity(pauseid);
		pauseid = getNextId();
	}
	is_paused = !is_paused;
}

void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<SpriteView>& views
) {
	#pragma omp parallel for
	for (int i = 0; i < anims.n_components; ++i) {
		SpriteAnim& anim = anims.components[i];
		anim.since_last_update += TIME_STEP * !anim.paused;
		float frametime = 1000.f / anim.framerate;
		while (anim.since_last_update >= frametime) {
			anim.current_frame = anim.anim_graph[anim.current_frame];
			anim.since_last_update -= TIME_STEP;
		}
		views.getComponent(anim.id).src_rect = {
			anim.current_frame % anim.cols * anim.cellw,
			anim.current_frame / anim.cols * anim.cellh,
			anim.cellw,
			anim.cellh
		};
	}
}

void updateLinearVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<LinearVelocity>& linear_velocities
) {
	#pragma omp parallel for
	for (int i = 0; i < linear_velocities.n_components; ++i) {
		LinearVelocity& v = linear_velocities.components[i];
		Transform& t = transforms.getNextComponent(v.id);
		t.pos = t.pos + v.velocity;
		v.velocity = v.acceleration + v.velocity * (1 - v.friction);
	}
}

void updateAngularVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<AngularVelocity>& angular_velocities
) {
	#pragma omp parallel for
	for (int i = 0; i < angular_velocities.n_components; ++i) {
		AngularVelocity& v = angular_velocities.components[i];
		Transform& t = transforms.getNextComponent(v.id);
		t.rot = t.rot + v.velocity;
		v.velocity = v.acceleration + v.velocity * (1 - v.friction);
	}
}

// Initialises sdl
bool init() {
	if (!initSdl()) {
		return false;
	}
	initGame();
	return true;
}

bool initSdl() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Failed to initiaze SDL. %s\n", SDL_GetError());
		return false;
	}
	i32 img_flags = IMG_INIT_PNG;
	if(!(IMG_Init(img_flags) & img_flags)) {
		printf("Failed to initialize SDL image. %s\n", IMG_GetError());
		return false;
	};
	return true;
}

void initGame() {
	ecs.registerComponentType<Transform>();
	ecs.registerComponentType<SpriteView>();
	ecs.registerComponentType<SpriteAnim>();
	ecs.registerComponentType<LinearVelocity>();
	ecs.registerComponentType<AngularVelocity>();

	u32 cameraid = getNextId();
	ecs.createComponent<Transform>(cameraid, { cameraid });
	rendering_system = new RenderingSystem(
		ecs.getComponentManager<Transform>(),
		ecs.getComponentManager<SpriteView>(),
		sprite_manager,
		Camera{ .id = cameraid, 
						.viewport = {0, 0, INTERNAL_WINDOW_WIDTH, INTERNAL_WINDOW_HEIGHT}
					}
	);
	
	// sprites init
	sprite_manager.loadSprite(snake_sprite_path);
	sprite_manager.loadSprite(apple_sprite_path);
	sprite_manager.loadSprite(board_sprite_path);
	sprite_manager.loadSprite(pause_sprite_path);

	// board init
	u32 board_id = getNextId();

	ecs.createEntity(board_id, 
		Transform	{ .id = board_id, 
								.scale = {31.25, 31.25}
							},
		SpriteView{ .id = board_id, 
								.sprite_path = board_sprite_path, 
								.src_rect = { 0, 0, sprite_manager.getSprite(board_sprite_path).w, sprite_manager.getSprite(board_sprite_path).h }, 
								.zindex = 9 
							}
	);

	// snake init
	BoardPos ss1_pos = { 3, BOARD_HEIGHT / 2 };
	BoardPos ss2_pos = { 4, BOARD_HEIGHT / 2 };
	u32 ss1_id = createSnakeSegment({ 0, ss1_pos, snakedir::LEFT, snakedir::RIGHT, STRAIGHT });
	createSnakeSegment({ 0, ss2_pos, snakedir::LEFT, snakedir::RIGHT, STRAIGHT });
	ecs.getc<Transform>(ss1_id).rot = 180;
	ecs.getNextc(ss1_id).rot = 180;

	SpriteAnim& ss1_anim = ecs.getc<SpriteAnim>(ss1_id);
	ss1_anim.anim_graph = { 0, 0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13, 14 };
	ss1_anim.current_frame = 15;
	ecs.getc<SpriteView>(ss1_id).src_rect = {
		.x = ss1_anim.current_frame % ss1_anim.cols * ss1_anim.cellw,
		.y = ss1_anim.current_frame / ss1_anim.cols * ss1_anim.cellh,
		.w = ss1_anim.cellw,
		.h = ss1_anim.cellh
	};
	snakedir::direction next_dir = snakedir::RIGHT;

	// apple init
	ecs.createEntity(apple_id,
		Transform	{ .id = apple_id 
							},
		SpriteView{ .id = apple_id, 
								.sprite_path = apple_sprite_path, 
								.src_rect = { 0, 0, 64, 64 }, 
								.zindex = 11 
							}
	);
	generateApplePos();
	changePause();
}

// Cleans up sdl
void close() {
	SDL_Quit();
}