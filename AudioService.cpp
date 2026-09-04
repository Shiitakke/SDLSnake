#include "AudioService.hpp"

IAudioSystem* AudioService::_service = nullptr;

void AudioService::registerService(IAudioSystem* service) {
  _service = service;
}

IAudioSystem* AudioService::service() {
  return _service;
};
