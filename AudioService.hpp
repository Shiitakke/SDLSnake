#pragma once

class IAudioSystem;

class AudioService {
public:
  static void registerService(IAudioSystem* service);
  static IAudioSystem* service();
private:
  static IAudioSystem* _service;
};
