#include "TitleScreenScene.hpp"
#include "World.hpp"

void TitleScreenScene::update() {
}

void TitleScreenScene::initialise(World& world, TextureManager& texture_manager) {
  auto& ecs = world.ecs;

  //load assets
  texture_manager.loadTexture(snake_logo_sprite_path);
  texture_manager.loadTexture(start_sprite_path);
  texture_manager.loadTexture(quit_sprite_path);
  texture_manager.loadTexture(arrow_sprite_path);
  texture_manager.loadTexture(board_sprite_path);

  //create background board
  board_id = ecs.createEntity(
    Transform{ .scale = {31.25, 31.25}
    },
    Sprite{ .sprite_path = board_sprite_path,
                .texture_id = texture_manager.getTextureid(board_sprite_path),
                .src_rect = { 0, 0, texture_manager.getTexture(board_sprite_path).w, texture_manager.getTexture(board_sprite_path).h },
                .zindex = 9
    }
  );

  //create snake logo
  snake_logo_id = ecs.createEntity(
    Transform{ .pos {.x = 0, .y = 100},
               .scale = {8, 8}
    },
    Sprite{ .sprite_path = snake_logo_sprite_path,
                .texture_id = texture_manager.getTextureid(snake_logo_sprite_path),
                .src_rect = { 0, 0, texture_manager.getTexture(snake_logo_sprite_path).w, texture_manager.getTexture(snake_logo_sprite_path).h },
                .zindex = 10
    }
  );
  //create start option
  start_option_id = ecs.createEntity(
    Transform{  .pos = {.x = 0, .y = -150},
                .scale = {3, 3}
    },
    Sprite{ .sprite_path = start_sprite_path,
                .texture_id = texture_manager.getTextureid(start_sprite_path),
                .src_rect = { 0, 0, texture_manager.getTexture(start_sprite_path).w, texture_manager.getTexture(start_sprite_path).h },
                .zindex = 10
    }
  );
  //create quit option
  quit_option_id = ecs.createEntity(
    Transform{  .pos = {.x = 0, .y = -300},
                .scale = {3, 3}
    },
    Sprite{ .sprite_path = quit_sprite_path,
                .texture_id = texture_manager.getTextureid(quit_sprite_path),
                .src_rect = { 0, 0, texture_manager.getTexture(quit_sprite_path).w, texture_manager.getTexture(quit_sprite_path).h },
                .zindex = 10
    }
  );
  //create arrow? (should this be left to the controller?)
  arrow_id = ecs.createEntity(
    Transform{  .pos = {.x = -200, .y = -150},
                .scale = {2, 2}
    },
    Sprite{ .sprite_path = arrow_sprite_path,
                .texture_id = texture_manager.getTextureid(arrow_sprite_path),
                .src_rect = { 0, 0, texture_manager.getTexture(arrow_sprite_path).w, texture_manager.getTexture(arrow_sprite_path).h },
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

void TitleScreenScene::deinitialise(World& world, TextureManager& texture_manager) {
  texture_manager.unloadTexture(snake_logo_sprite_path);
  texture_manager.unloadTexture(start_sprite_path);
  texture_manager.unloadTexture(quit_sprite_path);
  texture_manager.unloadTexture(arrow_sprite_path);
  texture_manager.unloadTexture(board_sprite_path);
  world.ecs.destroyEntity(snake_logo_id);
  world.ecs.destroyEntity(start_option_id);
  world.ecs.destroyEntity(quit_option_id);
  world.ecs.destroyEntity(arrow_id);
  world.ecs.destroyEntity(board_id);
  InputSystem::instance().popContext();
}

