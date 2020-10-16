#ifdef SAKURA_USE_SDL2

#include "platform.hpp"

#include "SDL.h"

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
