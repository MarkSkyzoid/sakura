#include "app.hpp"
#include "SDL.h"

#include "log/log.hpp"
#include "platform/input.hpp"

#pragma region DEBUG_DELETE
#include <array>
#include <algorithm>

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
	struct Player
	{
		f32 x = 0;
		f32 y = 0;
		f32 w = 40;
		f32 h = 40;
		f32 x_vel = 0;
		f32 y_vel = 0;
		f32 x_accel = 0;
		f32 y_accel = 0;
	};
	Player player;
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

#if 0
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
#endif
#pragma endregion DEBUG_DELETE_INPUT_TEST

#pragma region DEBUG_DELETE
		{
#define SAKURA_RGB 255, 183, 197
			SDL_SetRenderDrawColor(g_renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef SAKURA_RGB
			SDL_RenderClear(g_renderer);
			constexpr f32 PI = 3.14159265359;
			const auto dt = main_clock.delta_time_seconds();

			bool can_jump = false;
			const auto meters_to_pixels = 15.0f;

			// gravity
			if (player.y < config_.height - player.h) {
				player.y_accel += 9.8f;
			} else {
				player.y = config_.height - player.h;
				player.y_accel = 0.0;
				player.y_vel = 0.0;
				can_jump = true;
			}

			if (player.x <= 0 || player.x >= (config_.width - player.w)) {
				player.x_accel = 0.0;
				player.x_vel = 0.0;
			}

			if (player.x <= 0) {
				player.x += 0 - player.x;
			} else if (player.x >= (config_.width - player.w)) {
				player.x = config_.width - player.w;
			}
			const f32 max_x_accel = 100.0f * meters_to_pixels;
			{
				using namespace sakura::platform;
				if (curr_input_state.keyboard_state.key_states[(i32)Key::Right] == KeyState::Down) {
					player.x_accel = max_x_accel;
				} else if (curr_input_state.keyboard_state.key_states[(i32)Key::Left] == KeyState::Down) {
					player.x_accel = -max_x_accel;
				}

				if ((curr_input_state.keyboard_state.key_states[(i32)Key::Right] == KeyState::Up &&
					  prev_input_state.keyboard_state.key_states[(i32)Key::Right] == KeyState::Down) ||
					 (curr_input_state.keyboard_state.key_states[(i32)Key::Left] == KeyState::Up &&
					  prev_input_state.keyboard_state.key_states[(i32)Key::Left] == KeyState::Down)) {
					player.x_accel = 0;
					player.x_vel = 0;
				}
				if (can_jump && curr_input_state.keyboard_state.key_states[(i32)Key::Space] == KeyState::Down) {
					player.y_accel = -5000.0f;
				}
			}

			player.x_vel += std::min(meters_to_pixels, player.x_accel * dt);
			player.y_vel += player.y_accel * dt;
			player.x += player.x_vel * dt;
			player.y += player.y_vel * dt;
			
			if (!can_jump) {
				logging::log_info("player.y_accel %f", player.y_accel);
				logging::log_info("player.y_vel %f", player.y_vel);
			}

			SDL_FRect r;
			r.x = player.x;
			r.y = player.y;
			r.w = player.w;
			r.h = player.h;

			// Set render color to blue ( rect will be rendered in this color )
			SDL_SetRenderDrawColor(g_renderer, 178, 255, 178, 255);

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
