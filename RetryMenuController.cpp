#include "RetryMenuController.hpp"
#include "TitleScreenScene.hpp"
#include "util.hpp"
#include "assets.hpp"

RetryMenuController::RetryMenuController(
  World& _world,
  u32 _retry_logo_id,
  u32 _retry_option_id,
  u32 _retry_menu_id,
  u32 _retry_quit_id,
  u32 _arrow_id
) :
  world(_world),
  retry_logo_id(_retry_logo_id),
  retry_option_id(_retry_option_id),
  retry_menu_id(_retry_menu_id),
  retry_quit_id(_retry_quit_id),
  arrow_id(_arrow_id),
  arrow_transform(world.ecs.getNextc(arrow_id))
{
  selection_transforms[0] = world.ecs.getc<Transform>(retry_option_id);
  selection_transforms[1] = world.ecs.getc<Transform>(retry_menu_id);
  selection_transforms[2] = world.ecs.getc<Transform>(retry_quit_id);
  start_transform = arrow_transform;
  end_transform = arrow_transform;
  curr_transform = arrow_transform;
  timeline_id = world.timeline_manager.createTimeline({ .length = 0 });
}

void RetryMenuController::handleEvent(const SDL_Event& e) {

  if (e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
    case SDLK_LEFT:
      curr_selection = (std::max(curr_selection - 1, 0));
      changeSelection();
      break;
    case SDLK_RIGHT:
      curr_selection = (std::min(curr_selection + 1, 2));
      changeSelection();
      break;
    case SDLK_RETURN:
      switch (selections[curr_selection]) {
      case RetryMenuSelection::RETRY:
        break;
      case RetryMenuSelection::MENU:
        world.loadScene<TitleScreenScene>();

        //InputSystem::instance().popContext();
        break;
      case RetryMenuSelection::QUIT:
        SDL_Event quit_e;
        quit_e.type = SDL_QUIT;
        SDL_PushEvent(&quit_e);
        break;
      }
      break;
    }
  }
}

RetryMenuController::~RetryMenuController() {
  world.sprite_manager.unloadSprite(game_over_sprite_path);
  world.sprite_manager.unloadSprite(retry_sprite_path);
  world.sprite_manager.unloadSprite(menu_sprite_path);
  world.sprite_manager.unloadSprite(quit_sprite_path);
  world.sprite_manager.unloadSprite(arrow_sprite_path);

  world.timeline_manager.destroyTimelineChecked(timeline_id);
  
  world.ecs.destroyEntity(retry_logo_id);
  world.ecs.destroyEntity(retry_option_id);
  world.ecs.destroyEntity(retry_menu_id);
  world.ecs.destroyEntity(retry_quit_id);
  world.ecs.destroyEntity(arrow_id);
}

void RetryMenuController::changeSelection() {
  start_transform = arrow_transform;
  end_transform = selection_transforms[curr_selection];
  curr_transform = arrow_transform;
  world.timeline_manager.destroyTimelineChecked(timeline_id);
  world.timeline_manager.createTimeline({
    .length = 100,
    .on_update = [this](float delta)->void {moveArrow(delta);},
    .delta_curve = [](float delta)->float {return 4 * delta * std::pow(1.f / (1.f + delta), 2);}
    });
}

void RetryMenuController::moveArrow(float delta) {
  Transform curr_pos = lerp(start_transform, end_transform, delta);
  arrow_transform.pos = curr_pos.pos;
  arrow_transform.pos.x -= 150;
}