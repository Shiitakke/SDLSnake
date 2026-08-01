#include "TitleScreenScene.hpp"
#include "World.hpp"

void TitleScreenScene::update() {
}

void TitleScreenScene::initialise(World& world, SpriteManager& sprite_manager) {
  auto& ecs = world.ecs;

  //load assets
  sprite_manager.loadSprite(snake_logo_sprite_path);
  sprite_manager.loadSprite(start_sprite_path);
  sprite_manager.loadSprite(quit_sprite_path);
  sprite_manager.loadSprite(arrow_sprite_path);
  sprite_manager.loadSprite(board_sprite_path);

  //create background board
  board_id = ecs.createEntity(
    Transform{ .scale = {31.25, 31.25}
    },
    SpriteView{ .sprite_path = board_sprite_path,
                .sprite_id = sprite_manager.getSpriteid(board_sprite_path),
                .src_rect = { 0, 0, sprite_manager.getSprite(board_sprite_path).w, sprite_manager.getSprite(board_sprite_path).h },
                .zindex = 9
    }
  );

  //create snake logo
  snake_logo_id = ecs.createEntity(
    Transform{ .pos {.x = 0, .y = 100},
               .scale = {8, 8}
    },
    SpriteView{ .sprite_path = snake_logo_sprite_path,
                .sprite_id = sprite_manager.getSpriteid(snake_logo_sprite_path),
                .src_rect = { 0, 0, sprite_manager.getSprite(snake_logo_sprite_path).w, sprite_manager.getSprite(snake_logo_sprite_path).h },
                .zindex = 10
    }
  );
  //create start option
  start_option_id = ecs.createEntity(
    Transform{  .pos = {.x = 0, .y = -150},
                .scale = {3, 3}
    },
    SpriteView{ .sprite_path = start_sprite_path,
                .sprite_id = sprite_manager.getSpriteid(start_sprite_path),
                .src_rect = { 0, 0, sprite_manager.getSprite(start_sprite_path).w, sprite_manager.getSprite(start_sprite_path).h },
                .zindex = 10
    }
  );
  //create quit option
  quit_option_id = ecs.createEntity(
    Transform{  .pos = {.x = 0, .y = -300},
                .scale = {3, 3}
    },
    SpriteView{ .sprite_path = quit_sprite_path,
                .sprite_id = sprite_manager.getSpriteid(quit_sprite_path),
                .src_rect = { 0, 0, sprite_manager.getSprite(quit_sprite_path).w, sprite_manager.getSprite(quit_sprite_path).h },
                .zindex = 10
    }
  );
  //create arrow? (should this be left to the controller?)
  arrow_id = ecs.createEntity(
    Transform{  .pos = {.x = -200, .y = -150},
                .scale = {2, 2}
    },
    SpriteView{ .sprite_path = arrow_sprite_path,
                .sprite_id = sprite_manager.getSpriteid(arrow_sprite_path),
                .src_rect = { 0, 0, sprite_manager.getSprite(arrow_sprite_path).w, sprite_manager.getSprite(arrow_sprite_path).h },
                .zindex = 10
    }
  );
  //create controller, hand references to the controller
  IUserController* menu_controller = new TitleMenuController(
    ecs.getc<Transform>(start_option_id),
    ecs.getc<Transform>(quit_option_id),
    ecs.getNextc(arrow_id),
    world
  );
  InputSystem::instance().pushContext();
  InputSystem::instance().addToCurrentContext(menu_controller);
}

void TitleScreenScene::deinitialise(World& world, SpriteManager& sprite_manager) {
  sprite_manager.unloadSprite(snake_logo_sprite_path);
  sprite_manager.unloadSprite(start_sprite_path);
  sprite_manager.unloadSprite(quit_sprite_path);
  sprite_manager.unloadSprite(arrow_sprite_path);
  sprite_manager.unloadSprite(board_sprite_path);
  world.ecs.destroyEntity(snake_logo_id);
  world.ecs.destroyEntity(start_option_id);
  world.ecs.destroyEntity(quit_option_id);
  world.ecs.destroyEntity(arrow_id);
  world.ecs.destroyEntity(board_id);
  InputSystem::instance().popContext();
}

