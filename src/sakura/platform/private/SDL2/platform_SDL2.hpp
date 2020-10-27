#pragma once

#include "../../platform.hpp"
#include "../../input.hpp"
#include "SDL.h"

namespace sakura {
	struct Platform
	{
		Platform(const PlatformConfig& config) : config_(config) {}

		PlatformConfig config_;
		SDL_Window* window_ = nullptr;

		platform::InputState& get_current_input_state();
		platform::InputState& get_previous_input_state();

		/// <summary>
		///  Call at the beginning of the message pump
		/// </summary>
		void reset_input_state();

	private:
		i32 input_state_index_ = 0;
		platform::InputState input_states_[2];
	};
} // namespace sakura
