#include "app.hpp"
#include "log/log.hpp"
#include "SDL.h"

#include "../game_lib/game.hpp"

constexpr sakura::i32 WIDTH = 800;
constexpr sakura::i32 HEIGHT = 600;

SDL_Renderer* g_renderer = nullptr;

void init(const sakura::App& app)
{
	using namespace sakura;
	g_renderer = SDL_CreateRenderer((SDL_Window*)platform::get_native_window_handle(app.platform_handle()),
											  -1, SDL_RENDERER_SOFTWARE);

	sakura::game_lib::init(app);
}
void cleanup(const sakura::App& app)
{
	sakura::game_lib::cleanup(app);
	SDL_DestroyRenderer(g_renderer);
}
void fixed_update(sakura::f32 dt, const sakura::App& app)
{
	sakura::game_lib::fixed_update(dt, app);
}
void update(sakura::f32 dt, const sakura::App& app) { sakura::game_lib::update(dt, app); }
void render(sakura::f32 dt, sakura::f32 frame_interpolator, const sakura::App& app)
{
	sakura::game_lib::render(dt, frame_interpolator, app, g_renderer);
}

int main(int argc, char* argv[])
{
	sakura::App app = sakura::App::Builder()
							.set_title("Sakura")
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