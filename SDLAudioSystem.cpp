#include "SDLAudioSystem.hpp"

#include <SDL3_mixer/SDL_mixer.h>

#include "SDLSoundFactory.hpp"
#include "SoundManager.hpp"

SDLAudioSystem::SDLAudioSystem(
  SoundManager& _sound_manager
) :
  sound_manager(_sound_manager)
{
  if (!MIX_Init()) {
    printf("Failed to initialise SDL mixer. %s\m", SDL_GetError());
  };
  mixer = MIX_CreateMixerDevice(
    SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
    nullptr
  );
  MIX_CreateTrack(mixer);
  sound_manager.setSoundFactory(new SDLSoundFactory());
}

SDLAudioSystem::~SDLAudioSystem() {
  sound_manager.unloadAll();
  MIX_DestroyMixer(mixer);
  MIX_Quit();
}

void SDLAudioSystem::playSound(const std::string& path) {
  sound_queue.emplace(path);
}

void SDLAudioSystem::resolve() {
  for (std::string path : sound_queue) {
    if (!MIX_PlayAudio(
      mixer,
      static_cast<MIX_Audio*>(sound_manager.getSound(path))
    )) {
      printf("Could not play audio. %s\n", SDL_GetError());
    }
  }
  sound_queue.clear();
}