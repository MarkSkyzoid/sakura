#pragma once

#include "type_aliases.hpp"

#include <memory>

namespace sakura {
	struct PlatformConfig {
		const char* name;
		i32 width;
		i32 height;
	};

	class IPlatform {
	public:
		virtual ~IPlatform() = default;

		virtual void init(const PlatformConfig& config) = 0;
		virtual void cleanup() = 0;
	};

	namespace platform {
		/// <summary>
		/// Creates a new platform depending on the OS the code is compiled to.
		/// It gives ownership of the newly created object to the caller.
		/// </summary>
		/// <returns></returns>
		std::unique_ptr<IPlatform> create_platform();

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