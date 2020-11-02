#include "app.hpp"
#include "ecs/ecs.hpp"
#include "log/log.hpp"
#include "SDL.h"

#include <array>
#include <algorithm>

constexpr sakura::i32 WIDTH = 800;
constexpr sakura::i32 HEIGHT = 600;

SDL_Renderer* g_renderer = nullptr;

sakura::ecs::ECS world_ecs {};

void init(const sakura::App& app)
{
	g_renderer =
	SDL_CreateRenderer((SDL_Window*)sakura::platform::get_native_window_handle(app.platform_handle()),
							 -1, SDL_RENDERER_SOFTWARE);
}
void cleanup(const sakura::App& app) { SDL_DestroyRenderer(g_renderer); }
void fixed_update(sakura::f32 dt, const sakura::App& app) {}
void update(sakura::f32 dt, const sakura::App& app) {}
void render(sakura::f32 dt, sakura::f32 frame_interpolator, const sakura::App& app)
{
	auto render_system = [](sakura::ecs::ECS& ecs_instance, float delta_time, float interpolator,
									SDL_Renderer* renderer) {
#define SAKURA_RGB 255, 183, 197
		SDL_SetRenderDrawColor(renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef SAKURA_RGB
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	};

	render_system(world_ecs, dt, frame_interpolator, g_renderer);

	{
		sakura::logging::log_info("FPS: %f", 1.0f / dt);
	}
}

int main(int argc, char* argv[])
{

	sakura::App app = sakura::App::Builder()
							.set_title("Sakura Editor")
							.set_width(WIDTH)
							.set_height(HEIGHT)
							.set_target_frame_rate(60.0f)
							.set_init_callback(init)
							.set_cleanup_callback(cleanup)
							.set_fixed_update_callback(fixed_update)
							.set_update_callback(update)
							.set_render_callback(render);

	app.run();

	return 0;
}