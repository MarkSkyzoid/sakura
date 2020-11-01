#ifdef SAKURA_USE_SDL2

#include "platform_SDL2.hpp"
#include "log/log.hpp"

#include <array>

namespace sakura {
	namespace platform {
		i32 KeyCodes[] = {
#define DEFINE_KEY(name, value) value,
#include "../keyboard_codes.hpp"
#undef DEFINE_KEY
		};

		i32 KeyboardState::number_of_keys = static_cast<i32>(std::size(KeyCodes));
	}; // namespace platform

	sakura::platform::InputState& sakura::Platform::get_current_input_state()
	{
		return input_states_[input_state_index_];
	}

	sakura::platform::InputState& sakura::Platform::get_previous_input_state()
	{
		return input_states_[input_state_index_ ^ 1];
	}
	void sakura::Platform::reset_input_state()
	{
		input_state_index_ ^= 1;

		// This gets copied so that we carry the previous frame's state.
		// e.g. in the previous frame we were pressing a key.
		// Now we are still pressing it, but since SDL only accounts for key up and down events when they occur,
		// we need to carry the previous frame's state to let the system know we are still pressing that key.
		get_current_input_state() = get_previous_input_state();
	}

	namespace platform {
		const InputState& get_current_input_state(const PlatformHandle& handle)
		{
			return handle->get_current_input_state();
		}

		const InputState& get_previous_input_state(const PlatformHandle& handle)
		{
			return handle->get_previous_input_state();
		}

		const char* get_name_from_key(Key key)
		{
			auto index = static_cast<std::underlying_type_t<Key>>(key);
#define DEFINE_KEY(name, value) \
	if (index == value)          \
		return #name;
#include "../keyboard_codes.hpp"
#undef DEFINE_KEY

			return "";
		}

	} // namespace platform
} // namespace sakura

#endif // SAKURA_USE_SDL2