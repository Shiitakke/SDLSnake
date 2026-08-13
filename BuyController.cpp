#include "BuyController.hpp"

#include "InputSystem.hpp"
#include "ShopMenuController.hpp"
#include "SnakeGameScene.hpp"
#include "util.hpp"
#include "World.hpp"

BuyController::BuyController(
  SnakeGameScene& _game_scene, 
  World& _world
) : 
  game_scene(_game_scene), 
  world(_world) 
{

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
	Transform end_transform = camera_transform;
	end_transform.pos.y -= 1000;
	InputSystem::instance().pushContext(); // stop this from running twice
	game_scene.paused = true;
	// Timeline moves, timer creates buymenucontroller only when move is finished
	world.timeline_manager.createTimeline(
		Timeline{ .length = 1000,
							.on_update = 
								[&camera_transform, end_transform](float delta)->void {
									camera_transform = lerp(camera_transform, end_transform, delta);
								},
							.delta_curve = [](float delta)->float {return 4 * delta * std::pow(1.f / (1.f + delta), 2);}
		}
	);
	world.timer_manager.createTimer(
		Timer{ .interval = 1000,
						.callback = 
							[=]()->void {
								IUserController* shop_menu_controller = new ShopMenuController(game_scene, world);
								InputSystem::instance().addToCurrentContext(shop_menu_controller);
							},
						.repeats = false,
					}
	);
}