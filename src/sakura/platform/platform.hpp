#pragma once

#include "type_aliases.hpp"
#include <functional>
#include <memory>

namespace sakura {
	struct PlatformConfig
	{
		const char* title;
		i32 width;
		i32 height;
		bool resizable;

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
		const std::unique_ptr<Platform>& operator->() const { return handle_; }

	private:
		std::unique_ptr<Platform> handle_;
	};

	namespace platform {
		/// <summary>
		/// Creates a new platform depending on the OS the code is compiled to.
		/// It gives ownership of the newly created object to the caller (through a PlatformHandle object).
		/// </summary>
		/// <returns></returns>
		PlatformHandle create_platform(const PlatformConfig& config);

		// Platform interface

		/// <summary>
		/// Initialize the platform with the given configuration
		/// </summary>
		void init(PlatformHandle& handle);
		/// <summary>
		/// Call this when you are done using the platform
		/// </summary>
		void cleanup(PlatformHandle& handle);
		/// <summary>
		/// Handles platform specific event pump
		/// </summary>
		void do_message_pump(PlatformHandle& handle, void (*callback)(void* data) /* #SK_TODO: DELETE */);
		/// <summary>
		/// Set the main window's title
		/// </summary>
		void set_window_title(PlatformHandle& handle, const char* title);
		/// <summary>
		/// Retrieve the window's title
		/// </summary>
		const char* get_window_title(const PlatformHandle& handle);
		/// <summary>
		/// Returns a structure with a platform specific window handle (e.g. SDL_Window)
		/// </summary>
		void* get_native_window_handle(const PlatformHandle& handle);

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
	} // namespace platform
} // namespace sakura