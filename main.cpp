#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>

#include <stdio.h>

#include "common.hpp"
#include "util.hpp"

#include "World.hpp"
#include "assets.hpp"

#include "SnakeGameScene.hpp"
#include "TitleScreenScene.hpp"

#include "RenderingSystem.hpp"
#include "EntityComponentSystem.hpp"
#include "InputSystem.hpp"

#include "ComponentManager.hpp"

#include "Transform.hpp"
#include "SpriteView.hpp"
#include "SpriteAnim.hpp"
#include "LinearVelocity.hpp"
#include "AngularVelocity.hpp"

bool init();
bool initSdl();
void initGame();
void close();

void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<SpriteView>& views
);
void updateLinearVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<LinearVelocity>& linear_velocities
);
void updateAngularVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<AngularVelocity>& angular_velocities
);

// Engine stuff
bool quit = false;
World world;
RenderingSystem* rendering_system;

int main(int argc, char* args[]) {
	if (!init()) {
		return 1;
	}

	//Sdl loop init
	SDL_Event e;
	u64 start = SDL_GetTicks64();
	u64 end = start;
	float period = 0;
	
	while (quit == false) {

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
			
			ComponentManager<Transform>& transforms = world.ecs.getComponentManager<Transform>();
			transforms.components = transforms.next_transforms;

			world.updateScene();

			updateAnims(
				world.ecs.getComponentManager<SpriteAnim>(),
				world.ecs.getComponentManager<SpriteView>()
			);
		
			updateLinearVelocities(
				world.ecs.getComponentManager<Transform>(),
				world.ecs.getComponentManager<LinearVelocity>()
			);
			updateAngularVelocities(
				world.ecs.getComponentManager<Transform>(),
				world.ecs.getComponentManager<AngularVelocity>()
			);
			
			period -= TIME_STEP;			
		}
		rendering_system->render(period / TIME_STEP);
		end = SDL_GetTicks64();
		period += (end - start) * GAME_SPEED_SCALAR;
		start = end;
	}
	close();
	return 0;
}

void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<SpriteView>& views
) {
	#pragma omp parallel for
	for (i32 i = 0; i < anims.n_components; ++i) {
		SpriteAnim& anim = anims.components[i];
		anim.since_last_update += TIME_STEP * !anim.paused;
		float frametime = 1000.f / anim.framerate;
		while (anim.since_last_update >= frametime) {
			anim.current_frame = anim.anim_graph[anim.current_frame];
			anim.since_last_update -= TIME_STEP;
		}
		views.getComponent(anims.idFromIndex(i)).src_rect = {
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

bool init() {
	if (!initSdl()) {
		return false;
	}
	initGame();
	return true;
}

bool initSdl() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Failed to initiaze SDL. %s\n", SDL_GetError());
		return false;
	}
	i32 img_flags = IMG_INIT_PNG;
	if(!(IMG_Init(img_flags) & img_flags)) {
		printf("Failed to initialize SDL image. %s\n", IMG_GetError());
		return false;
	};
	return true;
}

void initGame() {
	u32 cameraid = world.ecs.getNextId();
	world.ecs.createComponent<Transform>(cameraid, { });
	rendering_system = new RenderingSystem(
		world.ecs.getComponentManager<Transform>(),
		world.ecs.getComponentManager<SpriteView>(),
		world.sprite_manager,
		world.camera
	);
	world.loadScene<TitleScreenScene>();
}

// Cleans up sdl
void close() {
	SDL_Quit();
}