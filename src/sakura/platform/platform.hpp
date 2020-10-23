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

	struct Platform;
	class PlatformHandle
	{
	public:

		/// <summary>
		/// These constructors/destructors/assignments need to be defined in the platform specific .cpp file because the unique_ptr implementation needs a concrete type (which is defined there).
		/// </summary>
		PlatformHandle();
		PlatformHandle(std::unique_ptr<Platform> handle);
		PlatformHandle(PlatformHandle&& other) = default;
		~PlatformHandle();

		PlatformHandle& operator=(PlatformHandle&& other);

		operator bool() const { return handle_ != nullptr; }

		std::unique_ptr<Platform>& operator->() { return handle_; }
		const std::unique_ptr<Platform>& operator->() const { return handle_;  }

	private:
		std::unique_ptr<Platform> handle_;
	};

	namespace platform
	{
		/// <summary>
		/// Creates a new platform depending on the OS the code is compiled to.
		/// It gives ownership of the newly created object to the caller (through a PlatformHandle object).
		/// </summary>
		/// <returns></returns>
		PlatformHandle create_platform(const PlatformConfig& config);

		// Platform interface
		void init(PlatformHandle& handle);
		void cleanup(PlatformHandle& handle);
		void do_message_pump(PlatformHandle& handle);
		void* get_native_window_handle(PlatformHandle& handle);

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