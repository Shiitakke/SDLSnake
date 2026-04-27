#pragma once

#include <stdio.h>
#include <SDL.h>

#include "Window.hpp"

class Renderer {
  public:
    static Renderer& instance() {
      static Renderer instance;
      return instance;
    };
    Renderer(Renderer const&) = delete;
    void operator=(Renderer const&) = delete;
    SDL_Renderer* r = nullptr;
  private:
    Renderer() {
      r = SDL_CreateRenderer(Window::instance().w, -1, SDL_RENDERER_ACCELERATED);
      if (r == nullptr) {
        printf("Failed to create renderer. %s", SDL_GetError());
      }
    };
    ~Renderer() {
      SDL_DestroyRenderer(Renderer::instance().r);
      r = nullptr;
    }
};
