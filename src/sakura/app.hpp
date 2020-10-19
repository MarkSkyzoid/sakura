#pragma once

#include "platform.hpp"
#include "type_aliases.hpp"

#include <memory>

namespace sakura {
	struct AppConfig
	{
		const char* name;
		i32 width;
		i32 height;
	};

	class IPlatform;

	class App {
	public:
		class Builder;

		App(const AppConfig& config) : config_(config) {}

		void run();
		bool is_running() const { return is_running_; }
	private:
		void init();
		void cleanup();

		AppConfig config_;
		bool is_running_ = false;

		std::unique_ptr<IPlatform> platform_;
	};

	class App::Builder {
	public:
		Builder() = default;
		~Builder() = default;

		operator App() { return App(config_); }

		Builder& set_name(const char* name) { config_.name = name; return *this; }
		Builder& set_width(i32 width) { config_.width = width; return *this; }
		Builder& set_height(i32 height) { config_.height = height; return *this; }

	private:
		AppConfig config_;
	};
}