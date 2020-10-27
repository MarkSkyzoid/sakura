#include "app.hpp"
#include "SDL.h"

#include "log/log.hpp"
#include "platform/input.hpp"

#include <array>

#pragma region DEBUG_DELETE
SDL_Renderer* g_renderer = nullptr;
#pragma endregion DEBUG_DELETE

namespace sakura {
	static PlatformConfig platform_config_from_app_config(const AppConfig& app_config)
	{
		sakura::PlatformConfig out_config;
		out_config.name = app_config.name;
		out_config.width = app_config.width;
		out_config.height = app_config.height;

		return out_config;
	}
} // namespace sakura

void sakura::App::run()
{
	SKR_ASSERT_M(!is_running(), "App %s is already running", config_.name);

	// Init
	init();

#pragma region DEBUG_DELETE
	g_renderer = SDL_CreateRenderer((SDL_Window*)platform::get_native_window_handle(platform_), -1,
											  SDL_RENDERER_SOFTWARE);
	f32 x = 0;

	std::array<time::Seconds, platform::MAX_KEYS> key_timer {};
#pragma endregion DEBUG_DELETE

	// Run
	Clock main_clock(config_.target_frame_rate);
	main_clock.start_from(0.0);
	while (!is_exiting_) {
		Duration frame_duration;

		platform::do_message_pump(platform_);

#pragma region DEBUG_DELETE_INPUT_TEST
		const auto& curr_input_state = platform::get_current_input_state(platform_);
		const auto& prev_input_state = platform::get_previous_input_state(platform_);

		for (int i = 0; i < platform::KeyboardState::number_of_keys; i++) {
			const auto ideal_frames_number_for_timer = 4.0f * (1.0f / config_.target_frame_rate);
			if (curr_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
				 prev_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
				 key_timer[i] < ideal_frames_number_for_timer) {
				key_timer[i] += main_clock.delta_time_seconds() / main_clock.time_scale();
			} else if (curr_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
						  prev_input_state.keyboard_state.key_states[i] == platform::KeyState::Down) {
				logging::log_info("you are holding the %s key",
										platform::get_name_from_key(static_cast<platform::Key>(i)));
			} else if (curr_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
						  prev_input_state.keyboard_state.key_states[i] == platform::KeyState::Up) {

				key_timer[i] = 0.0f;
				logging::log_info("just pressed the %s key",
										platform::get_name_from_key(static_cast<platform::Key>(i)));
			}
		}
#pragma endregion DEBUG_DELETE_INPUT_TEST

#pragma region DEBUG_DELETE
		{
#define SAKURA_RGB 255, 183, 197
			SDL_SetRenderDrawColor(g_renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef SAKURA_RGB
			SDL_RenderClear(g_renderer);
			constexpr float PI = 3.14159265359;
			main_clock.set_time_scale(3.0);
			x += 1.0 * main_clock.delta_time_seconds();
			if (x > 5.0 && x < 8.0) {
				main_clock.set_time_scale(0.4);
			}

			if (x >= 4 * PI) {
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
#pragma endregion DEBUG_DELETE

		auto dt_seconds = frame_duration.get();
		main_clock.update(dt_seconds);
	}

#pragma region DEBUG_DELETE
	SDL_DestroyRenderer(g_renderer);
#pragma endregion DEBUG_DELETE

	// Cleanup
	cleanup();
}

void sakura::App::init()
{
	is_running_ = true;

	// Platform system
	auto platform_config = platform_config_from_app_config(config_);
	// This is fine because platform is owned by App. Platform won't outlive App.
	platform_config.exit_callback = [this]() { this->request_exit(); };
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

void sakura::App::request_exit() { is_exiting_ = true; }
