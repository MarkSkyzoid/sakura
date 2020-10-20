#include "app.hpp"

#include "SDL.h" 
#include "log/log.hpp"

static SDL_Renderer* g_renderer = nullptr;

namespace sakura {
	static PlatformConfig platform_config_from_app_config(const AppConfig& app_config) {
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
	while (!is_exiting_) {
		platform_->do_message_pump();
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

	auto platform_config = platform_config_from_app_config(config_);
	platform_config.exit_callback = [this]() {this->request_exit(); }; // This is fine because platform is owned by App. Platform won't outlive App.
	platform_ = platform::create_platform(platform_config);
	platform_->init();

	//g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_SOFTWARE);
}

void sakura::App::cleanup()
{
	platform_->cleanup();
	is_running_ = false;
}

void sakura::App::request_exit()
{
    is_exiting_ = true;
}
