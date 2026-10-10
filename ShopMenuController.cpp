#include "ShopMenuController.hpp"

#include "InputSystem.hpp"
#include "SnakeGameScene.hpp"
#include "Transform.hpp"
#include "util.hpp"
#include "World.hpp"

ShopMenuController::ShopMenuController(
  SnakeGameScene& _game_scene,
  World& _world,
	vec2 _game_scene_origin
) :
  game_scene(_game_scene),
  world(_world),
	game_scene_origin(_game_scene_origin)
{

};

void ShopMenuController::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_b:
			[[fallthrough]];
		case SDLK_ESCAPE:
			moveToGameScreen();
			break;
		}
	}
}

// Moves camera to game screen, and removes this ShopMenuController
void ShopMenuController::moveToGameScreen() {
	Transform& camera_transform = world.camera.next;
	Transform end_transform;
	end_transform.pos = game_scene_origin;
	// push to stop this from running twice, then we pop twice later on to return to the scene
	InputSystem::instance().pushContext();
	// Timeline moves, creates buymenucontroller only when move is finished
	world.timeline_manager.createTimeline(
		Timeline{ 
			.length = 1000,
			.on_update =
				[&camera_transform, end_transform](float delta)->void {
					camera_transform = lerp(camera_transform, end_transform, delta);
				},
			.on_end = [=]()->void {
				game_scene.paused = false;
				InputSystem::instance().popContext();
				InputSystem::instance().popContext();
			},
			.delta_curve = [](float delta)->float {return delta * delta * (3.0f - 2.0f * delta);}
		}
	);
}