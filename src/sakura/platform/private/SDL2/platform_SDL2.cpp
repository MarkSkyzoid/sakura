#ifdef SAKURA_USE_SDL2

#include "platform/platform.hpp"
#include "log/log.hpp"

#include "SDL.h"

namespace sakura 
{
	class PlatformSDL2 final : public IPlatform
	{
	public:
		PlatformSDL2(const PlatformConfig& config) : IPlatform(config) {}

		~PlatformSDL2() = default;

		void init() override
		{
			SKR_ASSERT_M(config_.width != 0 && config_.height != 0, "Can't initialize window with width = %d and height=%d !", config_.width, config_.height);

			SDL_Init(SDL_INIT_VIDEO);
			// #SK_TODO: Create platform window abstraction - https://github.com/MarkSkyzoid/sakura/issues/3
			window_ = SDL_CreateWindow(
				config_.name,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				config_.width,
				config_.height,
				0
			);
		}

		void cleanup() override
		{
			SDL_DestroyWindow(window_);
			SDL_Quit();
		}

		virtual void do_message_pump() override
		{
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0)
			{
                //User requests quit
                if (e.type == SDL_QUIT)
                {
					SKR_ASSERT(config_.exit_callback);
					config_.exit_callback();
                }
            }
		}

	private:
		SDL_Window* window_ = nullptr;
	};
}

std::unique_ptr<sakura::IPlatform> sakura::platform::create_platform(const PlatformConfig& config)
{
	return std::make_unique<PlatformSDL2>(config);
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
void sakura::platform::debug_break()
{
	SDL_TriggerBreakpoint();
}

void sakura::platform::report_assert(const char* file, i32 line, const char* msg, AssertIgnoreMode ignore_mode)
{
	const SDL_MessageBoxButtonData buttons[] =
	{
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Abort" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Continue" },
	};

	auto can_be_ignored = [](AssertIgnoreMode mode) { return mode == AssertIgnoreMode::CanIgnore; };
	const auto num_buttons = can_be_ignored(ignore_mode) ? SDL_arraysize(buttons) : 1; // Only show "Abort" if it can't be ignored.

	const SDL_MessageBoxData message_box_data =
	{
		SDL_MESSAGEBOX_ERROR, /* .flags */
		nullptr, /* .window */
		"Assert!", /* .title */
		msg, /* .message */
		num_buttons, /* .numbuttons */
		buttons, /* .buttons */
		nullptr /* .colorScheme */
	};

	int button_id = -1;
	if (SDL_ShowMessageBox(&message_box_data, &button_id) >= 0) 
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ASSERTION AT \n%s(%d): %s", file, line, msg);
		if (button_id == 0)
		{
			debug_break();
		}
	}
}

#endif // SAKURA_USE_SDL2
