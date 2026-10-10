#include <SDL.h>
#include <SDL_timer.h>

#include <stdio.h>
#include <chrono>
#include <unordered_map>
#include <unordered_set>


#include "assets.hpp"
#include "common.hpp"
#include "util.hpp"

#include "AudioService.hpp"
#include "CollisionBox.hpp"
#include "ComponentManager.hpp"
#include "EntityComponentSystem.hpp"
#include "IAIController.hpp"
#include "IAudioSystem.hpp"
#include "InputSystem.hpp"
#include "PhysicsSystem.hpp"
#include "SDLAudioSystem.hpp"
#include "SDLRenderer.hpp"
#include "SnakeGameScene.hpp"
#include "Sprite.hpp"
#include "SpriteAnim.hpp"
#include "TimelineManager.hpp"
#include "TimerManager.hpp"
#include "Transform.hpp"
#include "TitleScreenScene.hpp"
#include "Velocity.hpp"
#include "World.hpp"

void init();
void deinit();
void updateTimers(
	EntityComponentSystem& ecs,
	ComponentManager<Timer>& timers
);
void updateTimelines(
	TimelineManager& timeline_manager
);
void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<Sprite>& sprites
);
void updateVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<Velocity>& velocities
);

bool quit = false;
constexpr float game_speed_scalar = 1.0f;
World world;
IRenderer* rendering_system;
PhysicsSystem* physics_system;

int main(int argc, char* args[]) {
	init();

	u64 start =
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()
		).count();
	u64 end = start;
	float period = 0;

	SDL_Event e;	
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			InputSystem::instance().queueEvent(e);
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
				rendering_system->handleWindowResize();
			}
		}

		while (period > TIME_STEP) {
			InputSystem::instance().processEvents();
			
			for (IAIController* ai_controller : world.ai_controllers) {
				ai_controller->update();
			}

			ComponentManager<Transform>& transforms = world.ecs.getComponentManager<Transform>();
			transforms.components = transforms.next_transforms;

			if (!world.paused) {
				world.updateScene();
				updateTimers(
					world.ecs,
					world.ecs.getComponentManager<Timer>()
				);
				updateTimelines(
					world.timeline_manager
				);
				updateAnims(
					world.ecs.getComponentManager<SpriteAnim>(),
					world.ecs.getComponentManager<Sprite>()
				);
				updateVelocities(
					world.ecs.getComponentManager<Transform>(),
					world.ecs.getComponentManager<Velocity>()
				);
				physics_system->update();
				AudioService::service()->resolve();
			}
			period -= TIME_STEP;			
		}
		rendering_system->render(period / TIME_STEP);
		
		end = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()
		).count();

		period += (end - start) * game_speed_scalar;
		start = end;
	}
	deinit();
	return 0;
}

void updateTimers(
	EntityComponentSystem& ecs,
	ComponentManager<Timer>& timers
) {
	for (i32 i = 0; i < timers.n_components; ++i) {
		Timer& timer = timers.components[i];
		timer.since_last_update += TIME_STEP * !timer.paused;
		while (timer.since_last_update >= timer.interval) {
			u32 timerid = timers.idFromIndex(i);
			timer.since_last_update -= timer.interval;
			// timer callback may destroy itself via destroyEntity()
			// so we can save the callback, destroy it safely and we're good
			// if the callback destroys itself via destroyComponent<timer>, 
			// then thats just user error and lets not account for it
			std::function<void()> callback = timer.callback;
			if (!timer.repeats) {
				if (ecs.component_counts[timerid] == 1) {
					ecs.destroyEntity(timerid);
				} else {
					timers.destroyComponent(timerid);
				}
				--i;
			}
			callback();
		}
	}
}

void updateTimelines(
	TimelineManager& timeline_manager
) {
	for (i32 i = 0; i < timeline_manager.timelines.n_components; ++i) {
		Timeline& timeline = timeline_manager.timelines.components[i];
		timeline.curr += TIME_STEP;
		timeline.curr = std::min(timeline.curr, timeline.length);
		float timeline_delta = timeline.curr / timeline.length;
		timeline.on_update(timeline.delta_curve(timeline_delta));
		if (timeline_delta >= 1.0f) {
			timeline.on_end();
			timeline_manager.destroyTimeline(timeline.id);
			i--;
		}
	}
}

void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<Sprite>& sprites
) {
	#pragma omp parallel for
	for (i32 i = 0; i < anims.n_components; ++i) {
		SpriteAnim& anim = anims.components[i];
		anim.since_last_update += TIME_STEP * !anim.paused;
		float frametime = 1000.0f / anim.framerate;
		while (anim.since_last_update >= frametime) {
			anim.current_frame = anim.anim_graph[anim.current_frame];
			anim.since_last_update -= frametime;
		}
		sprites.getComponent(anims.idFromIndex(i)).src_rect = {
			anim.current_frame % anim.cols * anim.cellw,
			anim.current_frame / anim.cols * anim.cellh,
			anim.cellw,
			anim.cellh
		};
	}
}

void updateVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<Velocity>& velocities
) {
	#pragma omp parallel for
	for (i32 i = 0; i < velocities.n_components; ++i) {
		Velocity& v = velocities.components[i];
		Transform& t = transforms.getNextComponent(velocities.idFromIndex(i));
		v.linear_velocity.velocity = v.linear_velocity.acceleration + v.linear_velocity.velocity * (1 - v.linear_velocity.friction);
		t.pos = t.pos + v.linear_velocity.velocity;
		v.angular_velocity.velocity = v.angular_velocity.acceleration + v.angular_velocity.velocity * (1 - v.angular_velocity.friction);
		t.rot = t.rot + v.angular_velocity.velocity;
	}
}

void init() {
	rendering_system = new SDLRenderer(
		world.ecs.getComponentManager<Transform>(),
		world.ecs.getComponentManager<Sprite>(),
		world.texture_manager,
		world.camera,
		world.ecs.getComponentManager<CollisionBox>(),
		1000,
		1000
	);
	physics_system = new PhysicsSystem(
		world.ecs.getComponentManager<Transform>(),
		world.ecs.getComponentManager<CollisionBox>()
	);
	AudioService::registerService(
		new SDLAudioSystem(world.sound_manager)
	);
	world.loadScene<TitleScreenScene>();
}

void deinit() {
	delete rendering_system;
}
