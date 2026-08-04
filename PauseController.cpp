#include "PauseController.hpp"
#include "InputSystem.hpp"
#include "UnpauseController.hpp"

void PauseController::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				changePause();
				break;
		}
	}
}
// pause screen image init
void PauseController::changePause() {
	world.paused = true;
	Texture& pause_sprite = world.texture_manager.getTexture(pause_sprite_path);
	u32 pauseid = world.ecs.createEntity(
		Transform	{	.scale = {4, 4}
							},
		Sprite{ .sprite_path = pause_sprite_path,
								.sprite_id = world.texture_manager.getTextureid(pause_sprite_path),
								.src_rect = {0, 0, pause_sprite.w, pause_sprite.h},
								.zindex = 15
							}
	);
	IUserController* unpause_controller = new UnpauseController(
		snake,
		world,
		pauseid
	);
	InputSystem::instance().pushContext();
	InputSystem::instance().addToCurrentContext(unpause_controller);
}