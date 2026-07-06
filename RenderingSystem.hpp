#pragma once

#include <SDL.h>

#include "common.hpp"
#include "Transform.hpp"
#include "SpriteView.hpp"
#include "Camera.hpp"
#include "ComponentManager.hpp"
#include "SpriteManager.hpp"

class RenderingSystem {
public:
  RenderingSystem(
    ComponentManager<Transform>& _transforms,
    ComponentManager<SpriteView>& _views,
    SpriteManager& _sprite_manager,
    Camera _camera
  ) :
    transforms(_transforms),
    views(_views),
    sprite_manager(_sprite_manager),
    camera(_camera)
  {
    window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, INTERNAL_WINDOW_WIDTH, INTERNAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
      printf("Failed to create window. %s", SDL_GetError());
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
      printf("Failed to create renderer. %s", SDL_GetError());
    }
    sprite_manager.setRenderer(renderer);
  };
  ~RenderingSystem();

  void render(float delta);
  void handleWindowResize();
private:
  ComponentManager<Transform>& transforms;
  ComponentManager<SpriteView>& views;
  SpriteManager& sprite_manager;
  Camera camera;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  u32 internal_screen_width = INTERNAL_WINDOW_WIDTH;
  u32 internal_screen_height = INTERNAL_WINDOW_HEIGHT;
  float out_res_scale = 1.0f;
};

