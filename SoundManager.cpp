#include "SoundManager.hpp"

#include <SDL_mixer.h>
#include <string>
#include <stdio.h>


bool SoundManager::loadChunk(const std::string& path) {
  Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
  if (chunk == nullptr) {
    printf("Failed to load sfx. %s\n", Mix_GetError());
    return false;
  }
  chunk_map.emplace(path, chunk);
  return true;
}

void SoundManager::unloadChunk(const std::string& path) {
  Mix_FreeChunk(getChunk(path));
}

Mix_Chunk* SoundManager::getChunk(const std::string& path) {
  return chunk_map[path];
}