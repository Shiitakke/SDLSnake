#pragma once

#include "common.hpp"
#include "ComponentManager.hpp"
#include "IdManager.hpp"
#include "Timer.hpp"

class TimerManager {
public:

  u32 createTimer(Timer timer) {
    u32 id = id_manager.getNextId();
    timer.id = id;
    timers.createComponent(id, timer);
    return id;
  }

  void destroyTimer(u32 id) {
    timers.destroyComponent(id);
    id_manager.returnId(id);
  }

  void clearTimers() {
    timers.n_components = 0;
  }

  ComponentManager<Timer> timers;
  IdManager id_manager;
};

