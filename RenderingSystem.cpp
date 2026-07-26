#include "RenderingSystem.hpp"
#include "util.hpp"

RenderingSystem::~RenderingSystem() {
	sprite_manager.setRenderer(nullptr);
	sprite_manager.unloadAll();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void RenderingSystem::render(float delta) {
	SDL_RenderClear(renderer);

	SDL_Rect vp = { 0, 0, 0, 0 };
	SDL_RenderGetViewport(renderer, &vp);

	const Transform& cam_curr_t = transforms.getComponent(camera.id);
	const Transform& cam_next_t = transforms.getNextComponent(camera.id);
	const Transform cam_final_t = lerp(cam_curr_t, cam_next_t, delta);

	for (u32 i = 0; i < views.n_components; ++i) {
		const SpriteView& view = views.components[i];
		const Transform& curr_t = transforms.getComponent(views.idFromIndex(i));
		const Transform& next_t = transforms.getNextComponent(views.idFromIndex(i));
		const Transform t = lerp(curr_t, next_t, delta);
		SDL_Rect src_rect = view.src_rect;
		// Start at middle of viewport, translate by camera, then translate by position, center based on scale, multiply to scale from internal to actual size
		SDL_Rect dst_rect = {
			(internal_screen_width / 2 -
			(cam_final_t.pos.x * view.parallax_factor) + t.pos.x -
			(src_rect.w * t.scale.x / 2.f))
				* out_res_scale,

			(internal_screen_height / 2 +
			(cam_final_t.pos.y * view.parallax_factor) - t.pos.y -
			(src_rect.h * t.scale.y / 2.f))
				* out_res_scale,

			src_rect.w * t.scale.x * out_res_scale,
			src_rect.h * t.scale.y * out_res_scale
		};
		SDL_RenderCopyEx(
			renderer,
			sprite_manager.getSprite(view.sprite_path).t,
			&src_rect,
			&dst_rect,
			t.rot,
			NULL,
			view.flip_flags
		);
	}

	SDL_RenderPresent(renderer);
}

void RenderingSystem::handleWindowResize() {
	// Recalc the scaling for drawing
	// We render at an internal resolution
	// and then scale to the actual resolution
	// find the scaling factor
	i32 external_screen_width;
	i32 external_screen_height;
	SDL_GetWindowSize(window, &external_screen_width, &external_screen_height);

	// Fit to internal aspect ratio, use viewport to clip rendering
	float screen_width_factor = static_cast<float>(external_screen_width) / internal_screen_width;
	float screen_height_factor = static_cast<float>(external_screen_height) / internal_screen_height;
	out_res_scale = std::min(screen_width_factor, screen_height_factor);

	u32 vp_width = internal_screen_width * out_res_scale;
	u32 vp_height = internal_screen_height * out_res_scale;
	SDL_Rect vp = { (external_screen_width / 2) - vp_width / 2, (external_screen_height / 2) - vp_height / 2, vp_width, vp_height };
	SDL_RenderSetViewport(renderer, &vp);
}
