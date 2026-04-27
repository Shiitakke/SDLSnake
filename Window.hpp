#pragma once
#include <stdio.h>
#include <SDL.h>

#include "common.hpp"

class Window {
  public:
    static Window& instance() {
      static Window instance;
      return instance;
    };
    Window(Window const&) = delete;
    void operator=(Window const&) = delete;
    SDL_Window* w = nullptr;
  
  private:
    Window() {
      w = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
      if (w == nullptr) {
        printf("Failed to create window. %s", SDL_GetError());
      }
    };
    ~Window() {
      SDL_DestroyWindow(Window::instance().w);
      w = nullptr;
    }
};

