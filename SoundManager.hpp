#pragma once
#include <SDL_mixer.h>

#include <string>
#include <unordered_map>

class SoundManager {
public:
  SoundManager() = default;
  ~SoundManager() = default;
  bool loadChunk(const std::string& path);
  void unloadChunk(const std::string& path);
  Mix_Chunk* getChunk(const std::string& path);
private:
  std::unordered_map<std::string, Mix_Chunk*> chunk_map;
};

