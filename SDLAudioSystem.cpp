#include "SDLAudioSystem.hpp"

#include "SDLSoundFactory.hpp"
#include "SoundManager.hpp"

SDLAudioSystem::SDLAudioSystem(
  SoundManager& _sound_manager
) :
  sound_manager(_sound_manager)
{
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) {
    printf("Failed to initialise SDL mixer. %s\m", Mix_GetError());
  };
  Mix_AllocateChannels(64);
  sound_manager.setSoundFactory(new SDLSoundFactory());
}

SDLAudioSystem::~SDLAudioSystem() {
  sound_manager.unloadAll();
  Mix_CloseAudio();
  Mix_Quit();
}

void SDLAudioSystem::playSound(const std::string& path) {
  sound_queue.emplace(path);
}

void SDLAudioSystem::resolve() {
  for (std::string path : sound_queue) {
    if (
      Mix_PlayChannel(
        -1, 
        static_cast<Mix_Chunk*>(sound_manager.getSound(path)), 
        0
      ) == -1) {
      printf("Could not play audio. %s\n", Mix_GetError());
    };
  }
  sound_queue.clear();
}