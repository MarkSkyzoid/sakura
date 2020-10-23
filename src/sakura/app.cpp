#include "app.hpp"

#include "SDL.h" 
#include "log/log.hpp"

SDL_Renderer* g_renderer = nullptr;

namespace sakura 
{
	static PlatformConfig platform_config_from_app_config(const AppConfig& app_config)
	{
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

	// DEBUG DELETE
	g_renderer = SDL_CreateRenderer((SDL_Window*)platform::get_native_window_handle(platform_), -1, SDL_RENDERER_SOFTWARE);
	f32 x = 0;
	// .

	// Run
	Clock main_clock(config_.target_frame_rate);
	main_clock.start_from(0.0);
	while (!is_exiting_) 
	{
		Duration frame_duration;

		platform::do_message_pump(platform_);

		// DEBUG DELETE
		{
#define SAKURA_RGB 255, 183, 197
			SDL_SetRenderDrawColor(g_renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef  SAKURA_RGB
			SDL_RenderClear(g_renderer);
			constexpr float PI = 3.14159265359;
			main_clock.set_time_scale(3.0);
			x += 1.0 * main_clock.delta_time_seconds();
			if (x > 5.0 && x < 8.0)
			{
				main_clock.set_time_scale(0.4);
			}
			
			if (x >= 4 * PI)
			{
				x = 0.0;
			}

			SDL_FRect r;
			r.x = 400 + cos(x) * 160.0f;
			r.y = 300.0 + sin(x) * 160.0f;
			r.w = 10;
			r.h = 10;

			// Set render color to blue ( rect will be rendered in this color )
			SDL_SetRenderDrawColor(g_renderer, 122, 255, 145, 255);

			// Render rect
			SDL_RenderFillRectF(g_renderer, &r);

			SDL_RenderPresent(g_renderer);
		}
		// .

		auto dt_seconds = frame_duration.get();
		main_clock.update(dt_seconds);
	}

	// DEBUG DELETE
   SDL_DestroyRenderer(g_renderer);
	// .

	// Cleanup
	cleanup();
}

void sakura::App::init()
{
	is_running_ = true;

	// Platform system
	auto platform_config = platform_config_from_app_config(config_);
	platform_config.exit_callback = [this](){ this->request_exit(); }; // This is fine because platform is owned by App. Platform won't outlive App.
	platform_ = platform::create_platform(platform_config);
	platform::init(platform_);
}

void sakura::App::cleanup()
{
	SKR_ASSERT_FAST(platform_);
	platform::cleanup(platform_);
	is_running_ = false;
	is_exiting_ = false;
}

void sakura::App::request_exit()
{
    is_exiting_ = true;
}
