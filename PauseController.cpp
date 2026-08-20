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

void PauseController::changePause() {
	world.paused = true;
	world.texture_manager.loadTexture(pause_sprite_path);
	world.texture_manager.loadTexture(resume_sprite_path);
	world.texture_manager.loadTexture(menu_sprite_path);
	world.texture_manager.loadTexture(quit_sprite_path);
	world.texture_manager.loadTexture(arrow_sprite_path);
	// pause logo
	u32 retry_logo_id = world.ecs.createEntity(
		Transform{ 
			.pos {.x = 0, .y = 100},
			.scale = {.x = 4, .y = 4}
		},
		Sprite{ 
			.sprite_path = pause_sprite_path,
			.sprite_id = world.texture_manager.getTextureid(pause_sprite_path),
			.src_rect = {0, 0, world.texture_manager.getTexture(pause_sprite_path).w, world.texture_manager.getTexture(pause_sprite_path).h },
			.zindex = 12
		}
	);
	// retry option
	u32 retry_option_id = world.ecs.createEntity(
		Transform{ 
			.pos {.x = -300, .y = -150},
			.scale = {.x = 2, .y = 2}
		},
		Sprite{ 
			.sprite_path = resume_sprite_path,
			.sprite_id = world.texture_manager.getTextureid(resume_sprite_path),
			.src_rect = {0, 0, world.texture_manager.getTexture(resume_sprite_path).w, world.texture_manager.getTexture(resume_sprite_path).h },
			.zindex = 12
		}
	);
	// main menu option
	u32 retry_menu_id = world.ecs.createEntity(
		Transform{ 
			.pos {.x = 0, .y = -150},
			.scale = {.x = 2, .y = 2}

		},
		Sprite{ 
			.sprite_path = menu_sprite_path,
			.sprite_id = world.texture_manager.getTextureid(menu_sprite_path),
			.src_rect = {0, 0, world.texture_manager.getTexture(menu_sprite_path).w, world.texture_manager.getTexture(menu_sprite_path).h },
			.zindex = 12
		}
	);
	// quit option
	u32 retry_quit_id = world.ecs.createEntity(
		Transform{ 
			.pos {.x = 300, .y = -150},
			.scale = {.x = 2, .y = 2}
		},
		Sprite{ 
			.sprite_path = quit_sprite_path,
			.sprite_id = world.texture_manager.getTextureid(quit_sprite_path),
			.src_rect = {0, 0, world.texture_manager.getTexture(quit_sprite_path).w, world.texture_manager.getTexture(quit_sprite_path).h },
			.zindex = 12
		}
	);
	// selection arrow
	u32 arrow_id = world.ecs.createEntity(
		Transform{ 
			.pos {.x = -450, .y = -150},
			.scale = {.x = 2, .y = 2}
		},
		Sprite{ 
			.sprite_path = arrow_sprite_path,
			.sprite_id = world.texture_manager.getTextureid(arrow_sprite_path),
			.src_rect = {0, 0, world.texture_manager.getTexture(arrow_sprite_path).w, world.texture_manager.getTexture(arrow_sprite_path).h },
			.zindex = 12
		}
	);

	// retry screen controller
	IUserController* retry_controller = new UnpauseController(
		world,
		retry_logo_id,
		retry_option_id,
		retry_menu_id,
		retry_quit_id,
		arrow_id
	);
	InputSystem::instance().pushContext();
	InputSystem::instance().addToCurrentContext(retry_controller);
}
