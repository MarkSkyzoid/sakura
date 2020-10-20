#include "app.hpp"

#include "SDL.h" 
#include "log/log.hpp"

static SDL_Renderer* g_renderer = nullptr;

namespace sakura {
	static PlatformConfig app_config_to_platform_config(const AppConfig& app_config) {
		sakura::PlatformConfig out_config;
		out_config.name = app_config.name;
		out_config.width = app_config.width;
		out_config.height = app_config.height;

		return out_config;
	}
}

void sakura::App::run()
{
	SKR_ASSERT_M(!is_running(), "App %s is already running", config_.name);

	// Init
	init();

	// Run
	bool is_exiting = false;
	while (!is_exiting) {
		// #SK_TODO: Create platform layer message pump abstraction - https://github.com/MarkSkyzoid/sakura/issues/4
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				is_exiting = true;
			}
		}
//#define SAKURA_RGB 255, 183, 197
//		SDL_SetRenderDrawColor(g_renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
//#undef  SAKURA_RGB
//		SDL_RenderClear(g_renderer);
//		SDL_RenderPresent(g_renderer);
	}

	// Cleanup
	cleanup();
}

void sakura::App::init()
{
	is_running_ = true;

	platform_ = platform::create_platform();
	platform_->init(app_config_to_platform_config(config_));

	//g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_SOFTWARE);
}

void sakura::App::cleanup()
{
	platform_->cleanup();
}
