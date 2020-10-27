#pragma once

#include "type_aliases.hpp"

namespace sakura {
	class PlatformHandle;
	namespace platform {
		enum class Key : i32
		{
#define DEFINE_KEY(name, value) name = value,
#include "private/keyboard_codes.hpp"
#undef DEFINE_KEY
		};

		constexpr extern i32 MAX_KEYS = 300;

		enum class KeyState : u8
		{
			Up = 0,
			Down,
			Count,
		};

		struct KeyboardState
		{
			static i32 number_of_keys;
			KeyState key_states[MAX_KEYS];
		};
		struct InputState
		{
			KeyboardState keyboard_state;
		};

		/// <summary>
		/// Query the current frame's input state.
		/// NOTE: The input state gets update whenever platform::do_message_pump is called
		/// </summary>
		const InputState& get_current_input_state(const PlatformHandle& handle);
		/// <summary>
		/// Query the previous frame's input state.
		/// NOTE: The input state gets update whenever platform::do_message_pump is called
		/// </summary>
		const InputState& get_previous_input_state(const PlatformHandle& handle);

		const char* get_name_from_key(Key key);
	} // namespace platform
} // namespace sakura