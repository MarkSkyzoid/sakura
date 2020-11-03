#include "app.hpp"

#include "log/log.hpp"
#include "platform/input.hpp"

namespace sakura {
	static PlatformConfig platform_config_from_app_config(const AppConfig& app_config)
	{
		sakura::PlatformConfig out_config;
		out_config.title = app_config.title;
		out_config.width = app_config.width;
		out_config.height = app_config.height;
		out_config.resizable = app_config.resizable;
		return out_config;
	}
} // namespace sakura

void sakura::App::run()
{
	SKR_ASSERT_M(!is_running(), "App %s is already running", config_.title);

	// Init
	init();

	// Run
	const f64 fixed_dt = 1.0 / config_.target_frame_rate;
	f32 dt_accumulator = 0.0f;
	while (!is_exiting_) {
		Duration frame_duration;
		const auto frame_time = main_clock_.delta_time_seconds();
		dt_accumulator += frame_time;

		// Platform specific update
		platform::do_message_pump(platform_, config_.native_message_pump_callback /* #SK_TODO: DELETE */);

		// Fixed timestep update
		while (dt_accumulator >= fixed_dt) {
			config_.fixed_update_callback(fixed_dt, *this);

			dt_accumulator -= fixed_dt;
		}

		// Update
		config_.update_callback(frame_time, *this);

		// Render
		const f32 frame_interpolator = dt_accumulator / fixed_dt;
		config_.render_callback(frame_time, frame_interpolator, *this);

		auto dt_seconds = frame_duration.get();
		main_clock_.update(dt_seconds);
	}

	// Cleanup
	cleanup();
}

const char* sakura::App::window_title() const { return platform::get_window_title(platform_); }

void sakura::App::set_window_title(const char* title)
{
	platform::set_window_title(platform_, title);
}

void* sakura::App::native_window_handle() const { return platform::get_native_window_handle(platform_); }

void sakura::App::init()
{
	is_running_ = true;

	// Platform system
	auto platform_config = platform_config_from_app_config(config_);
	// This is fine because platform is owned by App. Platform won't outlive App.
	platform_config.exit_callback = [this]() { this->request_exit(); };
	platform_ = platform::create_platform(platform_config);
	platform::init(platform_);

	// Main clock is the main loop clock.
	// This clock is not meant to be messed with (e.g. with time scale) as it's the true clock.
	// You can have your own clocks to use, relative to this clock, for updating entities.
	main_clock_ = Clock { config_.target_frame_rate };
	main_clock_.start_from(0.0f);

	// At the very end call custom app's init callback.
	// This always needs to be the last thing here.
	config_.init_callback(*this);
}

void sakura::App::cleanup()
{
	// At the very beginning call custom app's cleanup callback.
	// This always needs to be the very first thing here (in reverse order from init).
	config_.cleanup_callback(*this);

	SKR_ASSERT_FAST(platform_);
	platform::cleanup(platform_);
	is_running_ = false;
	is_exiting_ = false;
}

void sakura::App::request_exit() { is_exiting_ = true; }

sakura::App::Builder::operator sakura::App() const
{
	SKR_ASSERT_FATAL_M(config_.init_callback != nullptr,
							 "Init Callback not set.\nCall .set_init_callback on the "
							 "App::Builder");
	SKR_ASSERT_FATAL_M(config_.cleanup_callback != nullptr,
							 "Cleanup Callback not set.\nCall .set_cleanup_callback on the "
							 "App::Builder");
	SKR_ASSERT_FATAL_M(config_.fixed_update_callback != nullptr,
							 "Fixed Update Callback not set.\nCall .set_fixed_update_callback on the "
							 "App::Builder");
	SKR_ASSERT_FATAL_M(config_.update_callback != nullptr,
							 "Update Callback not set.\nCall .set_update_callback on the App::Builder");
	SKR_ASSERT_FATAL_M(config_.render_callback != nullptr,
							 "Render Callback not set.\nCall .set_render_callback on the App::Builder");

	return App(config_);
}