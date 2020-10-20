#ifdef SAKURA_USE_SDL2

#include "platform/platform.hpp"
#include "log/log.hpp"

#include "SDL.h"

namespace sakura {
	class PlatformSDL2 final : public IPlatform {
	public:
		~PlatformSDL2() = default;

		void init(const PlatformConfig& config) override
		{
			SKR_ASSERT(config.height != 0 && config.width != 0);

			SDL_Init(SDL_INIT_VIDEO);
			// #SK_TODO: Create platform window abstraction - https://github.com/MarkSkyzoid/sakura/issues/3
			window_ = SDL_CreateWindow(
				config.name,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				config.width,
				config.height,
				0
			);
		}

		void cleanup() override
		{
			SDL_DestroyWindow(window_);
			SDL_Quit();
		}

	private:
		SDL_Window* window_ = nullptr;
	};
}

std::unique_ptr<sakura::IPlatform> sakura::platform::create_platform()
{
	return std::make_unique<PlatformSDL2>();
}

// Debugging
void sakura::platform::debug_break() {
	SDL_TriggerBreakpoint();
}

// Logging and displaying
void sakura::platform::log(Verbosity verbosity, const char* msg)
{
	SDL_LogPriority priority = SDL_LogPriority::SDL_LOG_PRIORITY_INFO;
	switch (verbosity)
	{
	case sakura::platform::Verbosity::Info:
		priority = SDL_LogPriority::SDL_LOG_PRIORITY_INFO;
		break;
	case sakura::platform::Verbosity::Warning:
		priority = SDL_LogPriority::SDL_LOG_PRIORITY_WARN;
		break;
	case sakura::platform::Verbosity::Critical:
		priority = SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL;
		break;
	case sakura::platform::Verbosity::Count: // Done on purpose.
	default:
		break;
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, priority, msg);
}

#endif // SAKURA_USE_SDL2
