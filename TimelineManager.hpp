#pragma once
#pragma once

#include "common.hpp"
#include "ComponentManager.hpp"
#include "IdManager.hpp"
#include "Timeline.hpp"

class TimelineManager {
public:
  u32 createTimeline(Timeline timeline) {
    u32 id = id_manager.getNextId();
    timeline.id = id;
    timelines.createComponent(id, timeline);
    return id;
  }

  void destroyTimeline(u32 id) {
    timelines.destroyComponent(id);
    id_manager.returnId(id);
  }

  void destroyTimelineChecked(u32 id) {
    if (timelines.exists(id)) {
      timelines.destroyComponent(id);
      id_manager.returnId(id);
    }
  }

  void clearTimelines() {
    timelines.n_components = 0;
  }

  ComponentManager<Timeline> timelines;
  IdManager id_manager;
};

