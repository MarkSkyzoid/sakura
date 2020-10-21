#include "app.hpp"

#include "SDL.h" 
#include "log/log.hpp"

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

	// Run
	Clock main_clock(config_.target_frame_rate);
	main_clock.start_from(0.0);
	while (!is_exiting_) 
	{
		Duration frame_duration;

		platform_->do_message_pump();

		auto dt_seconds = frame_duration.get();
		main_clock.update(dt_seconds);
	}

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
	platform_->init();
}

void sakura::App::cleanup()
{
	SKR_ASSERT_FAST(platform_);
	platform_->cleanup();
	is_running_ = false;
	is_exiting_ = false;
}

void sakura::App::request_exit()
{
    is_exiting_ = true;
}
