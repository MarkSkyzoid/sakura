#pragma once

#include "platform/platform.hpp"
#include "time/time.hpp"
#include "type_aliases.hpp"

#include <memory>

namespace sakura {
	class App;
	struct AppConfig
	{
		// Defaults
		static constexpr const char* DEFAULT_TITLE = "Sakura Game";
		static constexpr i32 DEFAULT_WIDTH = 800;
		static constexpr i32 DEFAULT_HEIGHT = 600;
		static constexpr f32 DEFAULT_FRAME_RATE = 30.0f;

		// Fields
		const char* title = DEFAULT_TITLE;
		i32 width = DEFAULT_WIDTH;
		i32 height = DEFAULT_HEIGHT;
		bool resizable = false;
		f32 target_frame_rate = DEFAULT_FRAME_RATE;

		using InitCleanupCallbackType = void (*)(const App& app);
		using NativeMessagePumpCallback = void (*)(void* data); /* #SK_TODO: DELETE */
		using UpdateCallbackType = void (*)(f32 dt, const App& app);
		using RenderCallbackType = void (*)(f32 dt, f32 frame_interpolator, const App& app);
		InitCleanupCallbackType init_callback = nullptr;
		InitCleanupCallbackType cleanup_callback = nullptr;
		UpdateCallbackType fixed_update_callback = nullptr;
		UpdateCallbackType update_callback = nullptr;
		RenderCallbackType render_callback = nullptr;
		UpdateCallbackType end_of_main_loop_update = nullptr;
		/* #SK_TODO: DELETE */
		NativeMessagePumpCallback native_message_pump_callback = nullptr;
	};

	class IPlatform;

	class App
	{
	public:
		class Builder;

		App(const AppConfig& config) : config_(config) {}

		void run();
		bool is_running() const { return is_running_; }

		const AppConfig& config() const { return config_; }

		const PlatformHandle& platform_handle() const { return platform_; }
		const Clock& main_clock() const { return main_clock_; }

		const char* window_title() const;
		void set_window_title(const char* title);

		void* native_window_handle() const;

	private:
		void init();
		void cleanup();
		void request_exit();

		AppConfig config_;
		bool is_running_ = false;
		bool is_exiting_ = false;

		PlatformHandle platform_;
		Clock main_clock_;
	};

	class App::Builder
	{
	public:
		Builder() = default;
		~Builder() = default;

		operator App() const;

		Builder& set_title(const char* title)
		{
			config_.title = title;
			return *this;
		}
		Builder& set_width(i32 width)
		{
			config_.width = width;
			return *this;
		}
		Builder& set_height(i32 height)
		{
			config_.height = height;
			return *this;
		}
		Builder& set_resizable(bool resizable)
		{
			config_.resizable = resizable;
			return *this;
		}
		Builder& set_target_frame_rate(f32 frame_rate)
		{
			config_.target_frame_rate = frame_rate;
			return *this;
		}
		Builder& set_init_callback(AppConfig::InitCleanupCallbackType callback)
		{
			config_.init_callback = callback;
			return *this;
		}
		Builder& set_cleanup_callback(AppConfig::InitCleanupCallbackType callback)
		{
			config_.cleanup_callback = callback;
			return *this;
		}
		Builder& set_fixed_update_callback(AppConfig::UpdateCallbackType callback)
		{
			config_.fixed_update_callback = callback;
			return *this;
		}
		Builder& set_update_callback(AppConfig::UpdateCallbackType callback)
		{
			config_.update_callback = callback;
			return *this;
		}
		Builder& set_render_callback(AppConfig::RenderCallbackType callback)
		{
			config_.render_callback = callback;
			return *this;
		}
		Builder& set_end_of_main_loop_update_callback(AppConfig::UpdateCallbackType callback)
		{
			config_.end_of_main_loop_update = callback;
			return *this;
		}
		/* #SK_TODO: DELETE */
		Builder& set_native_message_pump_callback(AppConfig::NativeMessagePumpCallback callback)
		{
			config_.native_message_pump_callback = callback;
			return *this;
		}

	private:
		AppConfig config_;
	};
} // namespace sakura