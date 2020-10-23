#pragma once

#include "platform/platform.hpp"
#include "time/time.hpp"
#include "type_aliases.hpp"

#include <memory>

namespace sakura
{
	struct AppConfig
	{
		// Defaults
		static constexpr const char* DEFAULT_NAME = "Sakura Game";
		static constexpr i32 DEFAULT_WIDTH = 800;
		static constexpr i32 DEFAULT_HEIGHT = 600;
		static constexpr f32 DEFAULT_FRAME_RATE = 30.0f;

		// Fields
		const char* name = DEFAULT_NAME;
		i32 width = DEFAULT_WIDTH;
		i32 height = DEFAULT_HEIGHT;
		f32 target_frame_rate = DEFAULT_FRAME_RATE;
	};

	class IPlatform;

	class App 
	{
	public:
		class Builder;

		App(const AppConfig& config) : config_(config) {}

		void run();
		bool is_running() const { return is_running_; }

	private:
		void init();
		void cleanup();
		void request_exit();

		AppConfig config_;
		bool is_running_ = false;
		bool is_exiting_ = false;

		PlatformHandle platform_;
	};

	class App::Builder
	{
	public:
		Builder() = default;
		~Builder() = default;

		operator App() { return App(config_); }

		Builder& set_name(const char* name) { config_.name = name; return *this; }
		Builder& set_width(i32 width) { config_.width = width; return *this; }
		Builder& set_height(i32 height) { config_.height = height; return *this; }
		Builder& set_target_frame_rate(f32 frame_rate) { config_.target_frame_rate = frame_rate; return *this; }

	private:
		AppConfig config_;
	};
}