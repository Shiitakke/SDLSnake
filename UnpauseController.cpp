#include "UnpauseController.hpp"
#include "TitleScreenScene.hpp"
#include "SnakeGameScene.hpp"
#include "util.hpp"
#include "assets.hpp"

UnpauseController::UnpauseController(
  World& _world,
  u32 _retry_logo_id,
  u32 _retry_option_id,
  u32 _retry_menu_id,
  u32 _retry_quit_id,
  u32 _arrow_id
) :
  world(_world),
  pause_logo_id(_retry_logo_id),
  resume_option_id(_retry_option_id),
  menu_option_id(_retry_menu_id),
  quit_option_id(_retry_quit_id),
  arrow_id(_arrow_id)
{
  Transform arrow_transform = world.ecs.getNextc(arrow_id);
  selection_transforms[0] = world.ecs.getc<Transform>(resume_option_id);
  selection_transforms[1] = world.ecs.getc<Transform>(menu_option_id);
  selection_transforms[2] = world.ecs.getc<Transform>(quit_option_id);
  start_transform = arrow_transform;
  end_transform = arrow_transform;
}

void UnpauseController::handleEvent(const SDL_Event& e) {
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
      case RetryMenuSelection::RESUME:
        InputSystem::instance().popContext();
        break;
      case RetryMenuSelection::MENU:
        world.loadScene<TitleScreenScene>();
        break;
      case RetryMenuSelection::QUIT:
        SDL_Event quit_e;
        quit_e.type = SDL_QUIT;
        SDL_PushEvent(&quit_e);
        break;
      }
      break;
    case SDLK_ESCAPE:
      InputSystem::instance().popContext();
      break;
    }
  }
}

UnpauseController::~UnpauseController() {
  world.paused = false;
  world.timeline_manager.destroyTimelineChecked(timeline_id);
  
  world.ecs.destroyEntity(pause_logo_id);
  world.ecs.destroyEntity(resume_option_id);
  world.ecs.destroyEntity(menu_option_id);
  world.ecs.destroyEntity(quit_option_id);
  world.ecs.destroyEntity(arrow_id);
}

void UnpauseController::changeSelection() {
  start_transform = world.ecs.getNextc(arrow_id);
  end_transform = selection_transforms[curr_selection];
  end_transform.pos.x -= 150;
  // Pausing the game pauses timelines
  // So for now, just teleport and figure out a better solution to pausing later
  // Most likely will need to be resolved with a seperation of game world and ui world
  Transform& arrow_transform = world.ecs.getNextc(arrow_id);
  arrow_transform = end_transform;
  ///
  ///
  world.timeline_manager.destroyTimelineChecked(timeline_id);
  timeline_id = world.timeline_manager.createTimeline({
    .length = 100,
    .on_update = [this](float delta)->void {moveArrow(delta);},
    .delta_curve = [](float delta)->float {return 4 * delta * std::pow(1.f / (1.f + delta), 2);}
    });
}

void UnpauseController::moveArrow(float delta) {
  Transform curr_pos = lerp(start_transform, end_transform, delta);
  Transform& arrow_transform = world.ecs.getNextc(arrow_id);
  arrow_transform.pos = curr_pos.pos;
}