#include "SnakeObject.hpp"

#include "assets.hpp"
#include "CollisionBox.hpp"
#include "SnakeGameScene.hpp"
#include "Sprite.hpp"
#include "SpriteAnim.hpp"
#include "Transform.hpp"
#include "World.hpp"

SnakeObject::SnakeObject(
	World& _world,
	SnakeGameScene& _scene,
	float _speed,
	SpriteColorMod _snake_color
) :
	world(_world),
	scene(_scene),
	snake_color(_snake_color)
{
	setSpeed(_speed);
	// snake init
	BoardPos ss1_pos = { 3, 8 };
	BoardPos ss2_pos = { 4, 8 };
	u32 ss1_id = createSnakeSegment({ 0, ss1_pos, SnakeDir::LEFT, SnakeDir::RIGHT, SnakeSpriteType::STRAIGHT });
	createSnakeSegment({ 0, ss2_pos, SnakeDir::LEFT, SnakeDir::RIGHT, SnakeSpriteType::STRAIGHT });
	world.ecs.getc<Transform>(ss1_id).rot = 180;
	world.ecs.getNextc(ss1_id).rot = 180;

	SpriteAnim& ss1_anim = world.ecs.getc<SpriteAnim>(ss1_id);
	ss1_anim.anim_graph = { 0, 0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13, 14 };
	ss1_anim.current_frame = 15;
	world.ecs.getc<Sprite>(ss1_id).src_rect = {
		.x = ss1_anim.current_frame % ss1_anim.cols * ss1_anim.cellw,
		.y = ss1_anim.current_frame / ss1_anim.cols * ss1_anim.cellh,
		.w = ss1_anim.cellw,
		.h = ss1_anim.cellh
	};
	SnakeDir next_dir = SnakeDir::RIGHT;

};

SnakeObject::~SnakeObject() {
	for (SnakeSegment& ss : segments) {
		world.ecs.destroyEntity(ss.id);
	}
}

void SnakeObject::updateSnake() {
	since_last_update += TIME_STEP;

	// Clean head dir, dont go backwards
	SnakeSegment& head = segments.back();
	if (next_dir == head.in_dir) {
		next_dir = head.out_dir;
	}
	head.out_dir = next_dir;

	// Look at direction enum explicit values for why this works
	if (static_cast<i32>(head.out_dir) + static_cast<i32>(head.in_dir) == 0) {
		// Straight
		if (head.curr_type != SnakeSpriteType::STRAIGHT) {
			world.ecs.getc<Sprite>(head.id).flip_flags = SpriteFlip::NONE;
			world.ecs.getc<SpriteAnim>(head.id).current_frame += 8;
			head.curr_type = SnakeSpriteType::STRAIGHT;
		}
	}
	else {
		//curved
		// Set flipping for curved sprites (it runs left-to-up, clockwise)
		Sprite& hv = world.ecs.getc<Sprite>(head.id);
		switch (head.in_dir) {
		case SnakeDir::LEFT:
			hv.flip_flags = head.out_dir == SnakeDir::DOWN ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
			break;
		case SnakeDir::RIGHT:
			hv.flip_flags = head.out_dir == SnakeDir::UP ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
			break;
		case SnakeDir::UP:
			hv.flip_flags = head.out_dir == SnakeDir::LEFT ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
			break;
		case SnakeDir::DOWN:
			hv.flip_flags = head.out_dir == SnakeDir::RIGHT ? SpriteFlip::VERTICAL : SpriteFlip::NONE;
			break;
		}
		if (head.curr_type != SnakeSpriteType::CURVED) {
			world.ecs.getc<SpriteAnim>(head.id).current_frame -= 8;
			head.curr_type = SnakeSpriteType::CURVED;
		}
	}
	while (since_last_update > time_per_move) {
		// Get tail, get head, place new head, remove tail if head moved into an empty space
		SnakeSegment& head = segments.back();
		SnakeSegment& tail = segments.front();
		BoardPos new_pos = head.pos;
		SnakeDir in_dir;
		switch (next_dir) {
		case SnakeDir::UP:
			new_pos.y--;
			in_dir = SnakeDir::DOWN;
			break;
		case SnakeDir::DOWN:
			new_pos.y++;
			in_dir = SnakeDir::UP;
			break;
		case SnakeDir::LEFT:
			new_pos.x--;
			in_dir = SnakeDir::RIGHT;
			break;
		case SnakeDir::RIGHT:
			new_pos.x++;
			in_dir = SnakeDir::LEFT;
			break;
		default:
			new_pos.y--;
			in_dir = SnakeDir::DOWN;
			break;
		}
		// Bounds checking
		if (new_pos.x < 0 || new_pos.x >= 16 ||
			new_pos.y < 0 || new_pos.y >= 16)
		{
			scene.onDeath();
			return;
		}

		bool collected_apple = false;
		// Since an animation plays of the tail shrinking, it feels better
		// to remove the tail first
		if (scene.appleAt(new_pos)) {
			collected_apple = true;
			scene.collectAppleAt(new_pos);
		} else {
			destroySnakeSegment();
		}
		switch (board[new_pos.x][new_pos.y]) {
		case SnakeBoardTile::EMPTY: 
			if (!collected_apple) {
				// Set the new tail to play a reverse animation of the segment shrinking
				SnakeSegment& new_tail = segments.front();
				u32 new_tail_id = new_tail.id;
				SpriteAnim& new_tail_anim = world.ecs.getc<SpriteAnim>(new_tail_id);
				new_tail_anim.anim_graph = { 0, 0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13, 14 };
				new_tail_anim.since_last_update = 0;
				Transform& new_tail_transform = world.ecs.getc<Transform>(new_tail_id);
				Transform& new_tail_next = world.ecs.getNextc(new_tail_id);
				Sprite& new_tail_view = world.ecs.getc<Sprite>(new_tail_id);
				if (new_tail.curr_type == SnakeSpriteType::STRAIGHT) {
					new_tail_transform.rot += 180;
					new_tail_next.rot += 180;
					new_tail_anim.current_frame = 15;
				}
				else {
					new_tail_transform.rot += 90;
					new_tail_next.rot += 90;
					new_tail_anim.current_frame = 7;
					if (new_tail_view.flip_flags == SpriteFlip::VERTICAL) {
						new_tail_view.flip_flags = static_cast<SpriteFlip::SpriteFlip>(SpriteFlip::HORIZONTAL | SpriteFlip::VERTICAL);
					}
					else {
						new_tail_view.flip_flags = SpriteFlip::VERTICAL;
					}
				}
			}
			break;
		case SnakeBoardTile::SNAKE:
			// kill this snake
			scene.onDeath();
			break;
		}
		// remove collision from old head
		world.ecs.destroyComponent<CollisionBox>(segments.back().id);
		u32 new_segment_id = createSnakeSegment({ 0, new_pos, in_dir, next_dir, SnakeSpriteType::STRAIGHT });
		Transform& t = world.ecs.getc<Transform>(new_segment_id);
		Transform& t_next = world.ecs.getNextc(new_segment_id);
		switch (in_dir) {
		case SnakeDir::DOWN:
			t.rot += 90;
			t_next.rot += 90;
			[[fallthrough]];
		case SnakeDir::RIGHT:
			t.rot += 90;
			t_next.rot += 90;
			[[fallthrough]];
		case SnakeDir::UP:
			t.rot += 90;
			t_next.rot += 90;
		}
		since_last_update -= time_per_move;
	}
}

u32 SnakeObject::createSnakeSegment(SnakeSegment ss) {
	const auto& [t_x, t_y] = scene.boardPosToWorldPos(ss.pos);
	u32 new_segment_id = world.ecs.createEntity(
		Transform{.pos = {t_x, t_y},
							.scale = {2, 2}
		},
		Sprite{ .sprite_path = snake_sprite_path,
						.sprite_id = world.texture_manager.getTextureid(snake_sprite_path),
						.src_rect = {0, 0, 32, 32},
						.zindex = 11,
						.color_mod = snake_color
		},
		SpriteAnim{ .rows = 4, .cols = 4,
								.cellw = 32, .cellh = 32,
								.framerate = 1000 / static_cast<i32>(time_per_move / 8),
								.anim_graph = { 1, 2, 3, 4, 5, 6, 7, 7, 9, 10, 11, 12, 13, 14, 15, 15 },
								.current_frame = 8
		},
		CollisionBox{ .extent = {.x = 50, .y = 50},
									.channels = static_cast<CollisionChannel::Channel>(CollisionChannel::c1 | SnakeGameScene::snake_segment_collision_channel),
									.on_begin_collision = [this](u32 otherid)->void {
										if (world.ecs.getc<CollisionBox>(otherid).channels & SnakeGameScene::apple_particle_collision_channel) {
											scene.collectParticle(otherid);
										}
									}
		}
	);
	ss.id = new_segment_id;
	segments.emplace_back(ss);
	board[ss.pos.x][ss.pos.y] = SnakeBoardTile::SNAKE;
	return new_segment_id;
}

void SnakeObject::destroySnakeSegment() {
	u32 id = segments.front().id;
	world.ecs.destroyEntity(id);
	BoardPos pos = segments.front().pos;
	board[pos.x][pos.y] = SnakeBoardTile::EMPTY;
	segments.pop_front();
}

void SnakeObject::setSpeed(float new_speed) {
	// need to change speed, time per move, and anim speeds
	speed = new_speed;
	time_per_move = 1000. / speed;
	anim_framerate = 1000 / static_cast<i32>(time_per_move / 8);
	for (SnakeSegment& segment : segments) {
		world.ecs.getc<SpriteAnim>(segment.id).framerate = anim_framerate;
	}
}
