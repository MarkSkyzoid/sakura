#pragma once

#include "type_aliases.hpp"
#include <functional>
#include <memory>

namespace sakura
{
	struct PlatformConfig
	{
		const char* name;
		i32 width;
		i32 height;

		std::function<void(void)> exit_callback;
	};

	class IPlatform
	{
	public:
		IPlatform(const PlatformConfig& config) : config_(config) {}
		virtual ~IPlatform() = default;

		virtual void init() = 0;
		virtual void cleanup() = 0;
		virtual void do_message_pump() = 0;

	protected:
		PlatformConfig config_;
	};

	namespace platform
	{
		/// <summary>
		/// Creates a new platform depending on the OS the code is compiled to.
		/// It gives ownership of the newly created object to the caller.
		/// </summary>
		/// <returns></returns>
		std::unique_ptr<IPlatform> create_platform(const PlatformConfig& config);

		// Time
		u64 get_high_resolution_timer_cycles();
		u64 get_high_resolution_timer_frequency();

		// Debugging
		void debug_break();

		enum class AssertIgnoreMode : u8
		{
			CanIgnore = 0,
			CannotIgnore,
			Count
		};
		void report_assert(const char* file, i32 line, const char* msg, AssertIgnoreMode ignore_mode);
	}
}