#include "TitleMenuController.hpp"

#include <stdio.h>
#include "SnakeGameScene.hpp"

void TitleMenuController::handleEvent(const SDL_Event& e) {
  if (e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
      case SDLK_DOWN:
        if (current_selection == MenuSelection::START) {
          current_selection = MenuSelection::END;
          changeSelection();
        }
        break;
      case SDLK_UP:
        if (current_selection == MenuSelection::END) {
          current_selection = MenuSelection::START;
          changeSelection();
        }
        break;
      case SDLK_RETURN:
        if (current_selection == MenuSelection::START) {
          world.loadScene<SnakeGameScene>();
        } else {
          SDL_Event quit_e;
          quit_e.type = SDL_QUIT;
          SDL_PushEvent(&quit_e);
        }
        break;
    }
  }
}

void TitleMenuController::changeSelection() {
  if (current_selection == MenuSelection::START) {
    arrow_transform.pos.y = start_option_transform.pos.y;
  } else {
    arrow_transform.pos.y = quit_option_transform.pos.y;
  }
}
