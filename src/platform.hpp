#pragma once

#include "type_aliases.hpp"

namespace sakura {
	namespace platform {
		// Debugging
		void debug_break();

		// Logging and displaying
		enum class Verbosity : u8 {
			Info = 0,
			Warning,
			Critical,
			Count
		};

		
		void log(Verbosity verbosity, const char* msg);
	}
}