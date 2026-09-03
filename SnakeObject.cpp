#include "SnakeObject.hpp"

#include "assets.hpp"
#include "BoardObject.hpp"
#include "CollisionBox.hpp"
#include "LinearVelocity.hpp"
#include "SnakeGameScene.hpp"
#include "Sprite.hpp"
#include "SpriteAnim.hpp"
#include "Transform.hpp"
#include "util.hpp"
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
	init();
};

SnakeObject::~SnakeObject() {
	for (SnakeSegment& ss : segments) {
		world.ecs.destroyEntity(ss.id);
	}
	world.ecs.destroyEntity(head_collision_id);
}

// Moves if enough time has passed
void SnakeObject::updateSnake() {
	if (paused) {
		return;
	}	
	since_last_update += TIME_STEP;

	while (since_last_update > time_per_move) {
		// Get tail, get head, place new head, remove tail if head moved into an empty space
		auto [in_dir, new_pos] = getNextPos();
		// Bounds checking
		if (new_pos.x < 0 || new_pos.x >= 16 ||
			new_pos.y < 0 || new_pos.y >= 16)
		{
			killSnake();
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
		switch (board.board[new_pos.x][new_pos.y]) {
		case BoardObject::BoardTile::EMPTY:
			if (!collected_apple) {
				// Set the new tail to play a reverse animation of the segment shrinking
				reverseTailAnimation();
			}
			break;
		case BoardObject::BoardTile::SNAKE:
			// kill this snake
			killSnake();
			return;
		}
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
		prev_head_collision_pos = next_head_collision_pos;
		next_head_collision_pos = board.boardPosToWorldPos(segments.back().pos);
		createHeadCollisionTimeline();
		since_last_update -= time_per_move;
	}
}

// Creates a new head for the snake
u32 SnakeObject::createSnakeSegment(SnakeSegment ss) {
	const auto& [t_x, t_y] = board.boardPosToWorldPos(ss.pos);
	u32 new_segment_id = world.ecs.createEntity(
		Transform{
			.pos = {t_x, t_y},
			.scale = {2, 2}
		},
		Sprite{ 
			.sprite_path = snake_sprite_path,
			.texture_id = world.texture_manager.getTextureid(snake_sprite_path),
			.src_rect = {0, 0, 32, 32},
			.zindex = 11,
			.color_mod = snake_color
		},
		SpriteAnim{ 
			.rows = 4, .cols = 4,
			.cellw = 32, .cellh = 32,
			.framerate = 1000 / static_cast<i32>(time_per_move / 8),
			.anim_graph = { 1, 2, 3, 4, 5, 6, 7, 7, 9, 10, 11, 12, 13, 14, 15, 15 },
			.current_frame = 8
		}
	);
	ss.id = new_segment_id;
	segments.emplace_back(ss);
	board.board[ss.pos.x][ss.pos.y] = BoardObject::BoardTile::SNAKE;
	return new_segment_id;
}

// Destroys the tail of the snake
void SnakeObject::destroySnakeSegment() {
	u32 id = segments.front().id;
	world.ecs.destroyEntity(id);
	BoardObject::BoardPos pos = segments.front().pos;
	board.board[pos.x][pos.y] = BoardObject::BoardTile::EMPTY;
	segments.pop_front();
}

// Gets the current position of the head
BoardObject::BoardPos SnakeObject::getHeadPos() {
	if (segments.size() < 1) {
		return { 4, 8 };
	}
	return segments.back().pos;
}

// Gets the board the head will be in next move
std::tuple<SnakeObject::SnakeDir, BoardObject::BoardPos> SnakeObject::getNextPos() {
	SnakeSegment& head = segments.back();
	SnakeSegment& tail = segments.front();
	BoardObject::BoardPos new_pos = head.pos;
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
	return { in_dir, new_pos };
}

// Kills the snake, destroying all segments and respawning it a while later
void SnakeObject::killSnake() {
	// segments
	for (SnakeSegment& ss : segments) {
		world.ecs.destroyEntity(ss.id);
		board.board[ss.pos.x][ss.pos.y] = BoardObject::BoardTile::EMPTY;
	}
	segments.clear();
	paused = true;
	// head collision
	world.timeline_manager.destroyTimelineChecked(head_collision_timeline_id);
	world.ecs.destroyComponent<CollisionBox>(head_collision_id);
	World* world_ref = &world;
	world.timer_manager.createTimer(
		Timer{
			.interval = scene.respawn_timer + 1000,
			.callback = [=, head_c_id = (this->head_collision_id)]()->void{world_ref->ecs.destroyEntity(head_c_id);}
		}
	);
	// respawn timer
	world.timer_manager.createTimer(
		Timer	{ 
			.interval = scene.respawn_timer,
			.callback = [this]()->void {
				world.timer_manager.destroyTimer(running_collection_total_reset_timer_id);
				this->init();
				this->paused = false;
			}
		}
	);
}

void SnakeObject::init() {
	// snake init
	BoardObject::BoardPos ss1_pos = { 3, 8 };
	BoardObject::BoardPos ss2_pos = { 4, 8 };
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
	next_dir = SnakeDir::RIGHT;
	since_last_update = 0.0f;
	// head collision init
	head_collision_id = world.ecs.createEntity(
		Transform{
			.pos = board.boardPosToWorldPos(ss2_pos)
		}
	);
	world.ecs.createComponent<CollisionBox>( head_collision_id, 
		CollisionBox{
			.extent = {.x = 75, .y = 75},
			.channels = static_cast<CollisionChannel::Channel>(CollisionChannel::c1 | SnakeGameScene::snake_segment_collision_channel),
			.on_begin_collision = [=](u32 otherid)->void { // Collect apple particles
				if (world.ecs.exists<CollisionBox>(otherid) &&
						world.ecs.getc<CollisionBox>(otherid).channels & SnakeGameScene::apple_particle_collision_channel
				) {
					scene.collectParticle(otherid, head_collision_id, this);
				}
			}
		}
	);
	prev_head_collision_pos = board.boardPosToWorldPos(ss1_pos);
	next_head_collision_pos = board.boardPosToWorldPos(ss2_pos);
	createHeadCollisionTimeline();
	running_collection_total_reset_timer_id = world.timer_manager.createTimer(
		Timer{
			.interval = 500,
			.callback = [=]()->void {
				this->running_collection_total = 0;
			},
			.repeats = true,
		}
	);
}

// Animates a collisionbox to move alongside the head
void SnakeObject::createHeadCollisionTimeline() {
	head_collision_timeline_id = world.timeline_manager.createTimeline(
		Timeline{
			.length = time_per_move,
			.on_update = [=](float delta)->void {
				Transform& t = world.ecs.getNextc(head_collision_id);
				t.pos = lerp(prev_head_collision_pos, next_head_collision_pos, delta);
			},
		}
	);
}

// Reverses the animation of the tail segment
void SnakeObject::reverseTailAnimation() {
	SnakeSegment& new_tail = segments.front();
	SpriteAnim& new_tail_anim = world.ecs.getc<SpriteAnim>(new_tail.id);
	new_tail_anim.anim_graph = { 0, 0, 1, 2, 3, 4, 5, 6, 8, 8, 9, 10, 11, 12, 13, 14 };
	new_tail_anim.since_last_update = 0;
	Transform& new_tail_transform = world.ecs.getc<Transform>(new_tail.id);
	Transform& new_tail_next = world.ecs.getNextc(new_tail.id);
	Sprite& new_tail_view = world.ecs.getc<Sprite>(new_tail.id);
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

// Sets the next direction the snake will take
void SnakeObject::setDirection(SnakeDir new_direction) {
	// Clean head dir, dont go backwards
	if (segments.size() < 1) { // could be called when dead
		return;
	}
	SnakeSegment& head = segments.back();
	if (new_direction == head.in_dir) {
		new_direction = head.out_dir;
	}
	head.out_dir = new_direction;

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
	next_dir = new_direction;
}

// Sets the speed, time per move, and updates all animations for the new speed
void SnakeObject::setSpeed(float new_speed) {
	// need to change speed, time per move, and anim speeds
	speed = new_speed;
	time_per_move = 1000. / speed;
	anim_framerate = 1000 / static_cast<i32>(time_per_move / 8);
	for (SnakeSegment& segment : segments) {
		world.ecs.getc<SpriteAnim>(segment.id).framerate = anim_framerate;
	}
}

// Updates the number that appears when collecting apple particles
void SnakeObject::updateCollectionIndicator(
	u32 apple_particle_value,
	BitmapFont testfont
) {
	// Clear old indicator if it exists
	if (running_collection_indicator_exists) {
		for (u32 id : running_collection_indicator.sprite_ids) {
			world.ecs.destroyEntity(id);
		};
		for (u32 id : running_collection_indicator2.sprite_ids) {
			world.ecs.destroyEntity(id);
		};
		world.timer_manager.destroyTimer(running_collection_indicator_timer_id);
		world.timer_manager.destroyTimer(running_collection_indicator_timer_id2);
		running_collection_indicator_exists = false;
	};
	// Create new indicator
	// Setup words
	running_collection_indicator_exists = true;
	world.timer_manager.timers.components[world.timer_manager.timers.id_map[running_collection_total_reset_timer_id]].since_last_update = 0;
	running_collection_total += apple_particle_value;
	Transform word_transform = world.ecs.getc<Transform>(head_collision_id);
	float word_scale = std::cbrt(running_collection_total);
	word_transform.scale = { .x = word_scale + 1, .y = word_scale + 1 };
	word_transform.rot = 0;
	Sprite letter_format = {
		.zindex = 255,
		.color_mod = {0, 0, 0},
	};
	BitmapFontWord word = makeBitmapWord(testfont, world, std::to_string(running_collection_total), word_transform, letter_format);
	letter_format.color_mod = { 255, 255, 255 };
	word_transform.scale.x -= 1;
	word_transform.scale.y -= 1;
	BitmapFontWord word2 = makeBitmapWord(testfont, world, std::to_string(running_collection_total), word_transform, letter_format);
	running_collection_indicator = word;
	running_collection_indicator2 = word2;

	// Now setup motion and destruction timer
	for (u32 letter_id : word.sprite_ids) {
		world.ecs.createComponent<LinearVelocity>(letter_id,
			LinearVelocity{
				.velocity = {.x = 0, .y = 10},
				.friction = 0.2f,
			}
		);
	}
	running_collection_indicator_timer_id = world.timer_manager.createTimer(
		Timer{
			.interval = 600,
			.callback = [=]()->void {
				for (u32 letter_id : word.sprite_ids) {
					world.ecs.destroyEntity(letter_id);
				}
				running_collection_indicator_exists = false;
			}
		}
	);
	for (u32 letter_id : word2.sprite_ids) {
		world.ecs.createComponent<LinearVelocity>(letter_id,
			LinearVelocity{
				.velocity = {.x = 0, .y = 10},
				.friction = 0.2f,
			}
		);
	}
	running_collection_indicator_timer_id2 = world.timer_manager.createTimer(
		Timer{
			.interval = 600,
			.callback = [=]()->void {
				for (u32 letter_id : word2.sprite_ids) {
				world.ecs.destroyEntity(letter_id);
				running_collection_indicator_exists = false;					
				}
			}
		}
	);
}