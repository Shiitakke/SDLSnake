#include "SnakeController.hpp"

#include "SnakeObject.hpp"

void SnakeController::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
			case SDLK_UP:
				controlled_snake.next_dir = SnakeObject::SnakeDir::UP;
				break;
			case SDLK_DOWN:
				controlled_snake.next_dir = SnakeObject::SnakeDir::DOWN;
				break;
			case SDLK_LEFT:
				controlled_snake.next_dir = SnakeObject::SnakeDir::LEFT;
				break;
			case SDLK_RIGHT:
				controlled_snake.next_dir = SnakeObject::SnakeDir::RIGHT;
				break;
		}
	}
}
