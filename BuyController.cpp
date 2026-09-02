#include "BuyController.hpp"

#include "InputSystem.hpp"
#include "ShopMenuController.hpp"
#include "SnakeGameScene.hpp"
#include "util.hpp"
#include "World.hpp"

BuyController::BuyController(
  SnakeGameScene& _game_scene, 
  World& _world,
	vec2 _buy_screen_origin,
	vec2 _game_screen_origin
) : 
  game_scene(_game_scene), 
  world(_world),
	buy_screen_origin(_buy_screen_origin),
	game_screen_origin(_game_screen_origin)
{
	world.texture_manager.loadTexture(font_test);
	u32 font_id = world.texture_manager.getTextureid(font_test);
	BitmapFont test = BitmapFont{
		.texture_id = font_id,
		.font_path = font_test,
		.rows = 7,
		.cols = 13,
		.cellw = 6,
		.cellh = 10,
		.glyph_map = {0},
	};

	i32 i = 0;
	for (char c = 'A'; c <= 'Z'; ++c) {
		test.glyph_map[c] = i;
		++i;
	}
	i = 26;
	for (char c = 'a'; c <= 'z'; ++c) {
		test.glyph_map[c] = i;
		++i;
	}
	test.glyph_map[' '] = 88;

	Transform word_transform = Transform{
		.pos = buy_screen_origin,
		.scale = {.x = 10, .y = 10}
	};
	makeBitmapWord(test, world, "Hello", word_transform);
};

void BuyController::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_b:
			moveToBuyMenu();
			break;
		}
	}
}

void BuyController::moveToBuyMenu() {
	Transform& camera_transform = world.ecs.getNextc(world.camera.id);
	Transform end_transform;
	end_transform.pos = buy_screen_origin;
	InputSystem::instance().pushContext(); // stop this from running twice
	game_scene.paused = true;
	// Timeline moves, timer creates buymenucontroller only when move is finished
	world.timeline_manager.createTimeline(
		Timeline{ 
			.length = 1000,
			.on_update = 
				[&camera_transform, end_transform](float delta)->void {
					camera_transform = lerp(camera_transform, end_transform, delta);
				},
			.on_end = [=]()->void {
				IUserController* shop_menu_controller = new ShopMenuController(game_scene, world, game_screen_origin);
				InputSystem::instance().addToCurrentContext(shop_menu_controller);			
			},
			.delta_curve = [](float delta)->float {return 4 * delta * std::pow(1.f / (1.f + delta), 2);}
		}
	);
}