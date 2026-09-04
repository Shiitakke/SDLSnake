#include <SDL.h>
#include <SDL_timer.h>

#include <stdio.h>
#include <unordered_map>
#include <unordered_set>

#include "assets.hpp"
#include "common.hpp"
#include "util.hpp"

#include "AngularVelocity.hpp"
#include "AudioSystem.hpp"
#include "CollisionBox.hpp"
#include "ComponentManager.hpp"
#include "EntityComponentSystem.hpp"
#include "IAIController.hpp"
#include "InputSystem.hpp"
#include "LinearVelocity.hpp"
#include "PhysicsSystem.hpp"
#include "SDLRenderer.hpp"
#include "SnakeGameScene.hpp"
#include "Sprite.hpp"
#include "SpriteAnim.hpp"
#include "TimelineManager.hpp"
#include "TimerManager.hpp"
#include "Transform.hpp"
#include "TitleScreenScene.hpp"
#include "World.hpp"

void init();
void deinit();
void updateTimers(
	TimerManager& timer_manager
);
void updateTimelines(
	TimelineManager& timeline_manager
);
void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<Sprite>& sprites
);
void updateLinearVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<LinearVelocity>& linear_velocities
);
void updateAngularVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<AngularVelocity>& angular_velocities
);

bool quit = false;
constexpr float game_speed_scalar = 1.0f;
World world;
IRenderer* rendering_system;
PhysicsSystem* physics_system;

int main(int argc, char* args[]) {
	init();

	SDL_Event e;
	u64 start = SDL_GetTicks64();
	u64 end = start;
	float period = 0;
	
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
				updateTimers(world.timer_manager);
				updateTimelines(world.timeline_manager);
				updateAnims(
					world.ecs.getComponentManager<SpriteAnim>(),
					world.ecs.getComponentManager<Sprite>()
				);
				updateLinearVelocities(
					world.ecs.getComponentManager<Transform>(),
					world.ecs.getComponentManager<LinearVelocity>()
				);
				updateAngularVelocities(
					world.ecs.getComponentManager<Transform>(),
					world.ecs.getComponentManager<AngularVelocity>()
				);
				physics_system->update();
				AudioSystem::instance().resolve();
			}
			period -= TIME_STEP;			
		}
		rendering_system->render(period / TIME_STEP);
		end = SDL_GetTicks64();
		period += (end - start) * game_speed_scalar;
		start = end;
	}
	deinit();
	return 0;
}

void updateTimers(
	TimerManager& timer_manager
) {
	for (i32 i = 0; i < timer_manager.timers.n_components; ++i) {
		Timer& timer = timer_manager.timers.components[i];
		timer.since_last_update += TIME_STEP * !timer.paused;
		while (timer.since_last_update >= timer.interval) {
			timer.callback();
			timer.since_last_update -= timer.interval;
			if (!timer.repeats) {
				timer_manager.destroyTimer(timer.id);
				--i;
			}
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

void updateLinearVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<LinearVelocity>& linear_velocities
) {
	#pragma omp parallel for
	for (i32 i = 0; i < linear_velocities.n_components; ++i) {
		LinearVelocity& v = linear_velocities.components[i];
		Transform& t = transforms.getNextComponent(linear_velocities.idFromIndex(i));
		t.pos = t.pos + v.velocity;
		v.velocity = v.acceleration + v.velocity * (1 - v.friction);
	}
}

void updateAngularVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<AngularVelocity>& angular_velocities
) {
	#pragma omp parallel for
	for (i32 i = 0; i < angular_velocities.n_components; ++i) {
		AngularVelocity& v = angular_velocities.components[i];
		Transform& t = transforms.getNextComponent(angular_velocities.idFromIndex(i));
		t.rot = t.rot + v.velocity;
		v.velocity = v.acceleration + v.velocity * (1 - v.friction);
	}
}

void init() {
	u32 cameraid = world.ecs.getNextId();
	world.camera.id = cameraid;
	world.ecs.createComponent<Transform>(cameraid, { });
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
	AudioSystem::instance().setSoundManager(world.sound_manager);
	world.loadScene<TitleScreenScene>();
}

void deinit() {
	delete rendering_system;
}
