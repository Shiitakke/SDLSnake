#include <SDL3/SDL.h>

#include <stdio.h>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

#include "assets.hpp"
#include "common.hpp"
#include "util.hpp"

#define PUGIXML_NO_EXCEPTIONS
#include "pugixml.hpp"

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

#include "TestScene.hpp"
#include "EventSystem.hpp"
#include "Events.hpp"

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
// keep big state off stack
World world;
SDLRenderer rendering_system = SDLRenderer(
	world.ecs.getComponentManager<Transform>(),
	world.ecs.getComponentManager<Sprite>(),
	world.texture_manager,
	world.camera,
	world.ecs.getComponentManager<CollisionBox>(),
	1000,
	1000
);

int main(int argc, char* args[]) {
	// Engine initialisation
	PhysicsSystem physics_system = PhysicsSystem(
		world.ecs.getComponentManager<Transform>(),
		world.ecs.getComponentManager<CollisionBox>()
	);
	AudioService::registerService(
		new SDLAudioSystem(world.sound_manager)
	);
	// Game initialisation
	world.loadScene<TitleScreenScene>();

	// TODO: add creating InputMap from file
	// also move this input map creation logic somewhere else nice (maybe in scenes?)
	// not sure how i can relate strings to inputaction or sdl enums generically though
	// may require handing the creation function a giant map of strings or something
	{
		InputMap im;
		im.mapTo(SDL_GAMEPAD_BUTTON_DPAD_DOWN, InputAction::move_down);
		im.mapTo(SDL_GAMEPAD_BUTTON_DPAD_UP, InputAction::move_up);
		im.mapTo(SDL_GAMEPAD_BUTTON_DPAD_LEFT, InputAction::move_left);
		im.mapTo(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, InputAction::move_right);
		im.mapTo(SDL_GAMEPAD_BUTTON_START, InputAction::pause);
		im.mapTo(SDL_GAMEPAD_BUTTON_START, InputAction::unpause);
		im.mapTo(SDL_GAMEPAD_BUTTON_SOUTH, InputAction::select_item);

		im.mapTo(SDL_SCANCODE_UP, InputAction::move_up);
		im.mapTo(SDL_SCANCODE_DOWN, InputAction::move_down);
		im.mapTo(SDL_SCANCODE_LEFT, InputAction::move_left);
		im.mapTo(SDL_SCANCODE_RIGHT, InputAction::move_right);
		im.mapTo(SDL_SCANCODE_W, InputAction::move_up);
		im.mapTo(SDL_SCANCODE_S, InputAction::move_down);
		im.mapTo(SDL_SCANCODE_A, InputAction::move_left);
		im.mapTo(SDL_SCANCODE_D, InputAction::move_right);
		im.mapTo(SDL_SCANCODE_ESCAPE, InputAction::pause);
		im.mapTo(SDL_SCANCODE_ESCAPE, InputAction::unpause);
		im.mapTo(SDL_SCANCODE_RETURN, InputAction::select_item);

		im.mapTo(SDL_GAMEPAD_AXIS_LEFTX, InputAction::move_left_right);
		im.mapTo(SDL_GAMEPAD_AXIS_LEFTY, InputAction::move_up_down);

		InputSystem::instance().addInputMapping(im);
	}

	u64 start = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();
	u64 end = start;
	float period = 0;

	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			InputSystem::instance().queueSDLEvent(e);
			switch (e.type) {
			case SDL_EVENT_QUIT:
				quit = true;
				break;
			case SDL_EVENT_WINDOW_RESIZED:
				rendering_system.handleWindowResize();
				break;

			}
		}

		while (period > TIME_STEP) {
			//TODO: update InputSystem to support axes and allow different triggers
			// eg started, ended and continuing actionsk when holding or releasing buttons
			InputSystem::instance().processEvents();

			
			// TODO: make a real AI system
			// probably should have finite state machines and full behaviour trees
			for (IAIController* ai_controller : world.ai_controllers) {
				ai_controller->update();
			}

			rendering_system.updateState();

			if (!world.paused) {
				// TODO: figure out a better way to run generic code than this
				// Maybe having frame events is ideal, for example 'OnStartTick', 'OnEndTick'
				// anything can then just sub to these events, and granularity can be added easily
				world.updateScene();
				EventSystem::instance().pub<OnTickStart>({});
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
				// TODO: let physics system handle integration so it can manage its partition better
				// Important note, entities that are moved directly by modifying transform location
				// would still need to be handled somehow, events dont really cut it since the ecs
				// hands out direct references and doesnt use a setter or anything
				updateVelocities(
					world.ecs.getComponentManager<Transform>(),
					world.ecs.getComponentManager<Velocity>()
				);
				physics_system.update();
				AudioService::service()->resolve();
			}
			period -= TIME_STEP;			
		}
		rendering_system.render(period / TIME_STEP);
		
		end = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()
		).count();

		period += (end - start) * game_speed_scalar;
		start = end;
	}
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
		Transform& t = transforms.getComponent(velocities.idFromIndex(i));
		v.linear_velocity.velocity = v.linear_velocity.acceleration + v.linear_velocity.velocity * (1 - v.linear_velocity.friction);
		t.pos = t.pos + v.linear_velocity.velocity;
		v.angular_velocity.velocity = v.angular_velocity.acceleration + v.angular_velocity.velocity * (1 - v.angular_velocity.friction);
		t.rot = t.rot + v.angular_velocity.velocity;
	}
}
