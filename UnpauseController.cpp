#include "UnpauseController.hpp"
#include "InputSystem.hpp"

void UnpauseController::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				changePause();
				break;
		}
	}
}

// pause screen image init
void UnpauseController::changePause() {
	snake.paused = false;
	world.ecs.destroyEntity(pauseid);
	InputSystem::instance().popContext();
}