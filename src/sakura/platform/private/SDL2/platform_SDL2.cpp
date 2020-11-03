#ifdef SAKURA_USE_SDL2

#include "platform_SDL2.hpp"
#include "log/log.hpp"

namespace sakura {
	PlatformHandle::PlatformHandle(std::unique_ptr<Platform> handle) : handle_(std::move(handle)) {}
	PlatformHandle::PlatformHandle() : handle_(nullptr) {}
	PlatformHandle::~PlatformHandle() {}

	sakura::PlatformHandle& PlatformHandle::operator=(PlatformHandle&& other)
	{
		handle_ = std::move(other.handle_);
		return *this;
	}
} // namespace sakura

sakura::PlatformHandle sakura::platform::create_platform(const PlatformConfig& config)
{
	sakura::PlatformHandle handle(std::make_unique<Platform>(config));
	return handle;
}

void sakura::platform::init(PlatformHandle& handle)
{
	const PlatformConfig& config = handle->config_;
	SKR_ASSERT_M(config.width != 0 && config.height != 0,
					 "Can't initialize window with width = %d and height=%d !", config.width, config.height);

	SDL_Init(SDL_INIT_VIDEO);
	// #SK_TODO: Create platform window abstraction - https://github.com/MarkSkyzoid/sakura/issues/3
	handle->window_ = SDL_CreateWindow(config.title, SDL_WINDOWPOS_UNDEFINED,
												  SDL_WINDOWPOS_UNDEFINED, config.width, config.height, 0);
}

void sakura::platform::cleanup(PlatformHandle& handle)
{
	SDL_DestroyWindow(handle->window_);
	SDL_Quit();
}

static void handle_sdl_event(const SDL_Event& sdl_event, sakura::PlatformHandle& handle)
{
	//
	switch (sdl_event.type) {
	case SDL_KEYUP: {
		auto key_idx = static_cast<sakura::platform::Key>(sdl_event.key.keysym.scancode);
		handle->get_current_input_state().keyboard_state.key_states[static_cast<sakura::i32>(key_idx)] =
		sakura::platform::KeyState::Up;
		break;
	}
	case SDL_KEYDOWN: {
		auto key_idx = static_cast<sakura::platform::Key>(sdl_event.key.keysym.scancode);
		handle->get_current_input_state().keyboard_state.key_states[static_cast<sakura::i32>(key_idx)] =
		sakura::platform::KeyState::Down;
		break;
	}
	default: {
		break;
	}
	}
}

void sakura::platform::do_message_pump(PlatformHandle& handle, void (*callback)(void* data) /* #SK_TODO: DELETE */)
{
	// Reset input state for new frame
	handle->reset_input_state();

	bool call_quit = false;

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		// User requests quit
		if (e.type == SDL_QUIT) {
			call_quit = true;
			break;
		}

		handle_sdl_event(e, handle);

		// #SK_TODO: DELETE
		if (callback) {
			callback(&e);
		}
	}

	if (call_quit) {
		SKR_ASSERT(handle->config_.exit_callback);
		handle->config_.exit_callback();
	}
}

void sakura::platform::set_window_title(PlatformHandle& handle, const char* title)
{
	SDL_SetWindowTitle(handle->window_, title);
}

const char* sakura::platform::get_window_title(const PlatformHandle& handle)
{
	return SDL_GetWindowTitle(handle->window_);
}

void* sakura::platform::get_native_window_handle(const PlatformHandle& handle)
{
	return handle->window_;
}

// Time
sakura::u64 sakura::platform::get_high_resolution_timer_cycles()
{
	return SDL_GetPerformanceCounter();
}
sakura::u64 sakura::platform::get_high_resolution_timer_frequency()
{
	return SDL_GetPerformanceFrequency();
}

// Debugging
void sakura::platform::debug_break() { SDL_TriggerBreakpoint(); }

void sakura::platform::report_assert(const char* file, i32 line, const char* msg, AssertIgnoreMode ignore_mode)
{
	const SDL_MessageBoxButtonData buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Abort" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Continue" },
	};

	auto can_be_ignored = [](AssertIgnoreMode mode) { return mode == AssertIgnoreMode::CanIgnore; };
	const i32 num_buttons = can_be_ignored(ignore_mode) ? SDL_arraysize(buttons) :
																			1; // Only show "Abort" if it can't be ignored.

	const SDL_MessageBoxData message_box_data = {
		SDL_MESSAGEBOX_ERROR, /* .flags */
		nullptr,              /* .window */
		"Assert!",            /* .title */
		msg,                  /* .message */
		num_buttons,          /* .numbuttons */
		buttons,              /* .buttons */
		nullptr               /* .colorScheme */
	};

	int button_id = -1;
	if (SDL_ShowMessageBox(&message_box_data, &button_id) >= 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ASSERTION AT \n%s(%d): %s", file, line, msg);
		if (button_id == 0) {
			debug_break();
		}
	}
}

#endif // SAKURA_USE_SDL2