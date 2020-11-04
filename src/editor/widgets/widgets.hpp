#pragma once

#include "log_window.hpp"
#include "toolbar.hpp"

namespace sakura::editor {
	struct Widgets
	{
		widgets::LogWindow log_window;
		widgets::Toolbar toolbar;

		static void log_callback(void* user_data, const sakura::logging::Message& message)
		{
			using namespace widgets;
			LogWindow* log_window = static_cast<LogWindow*>(user_data);
			log_window->AddLog("[%s] %s\n", sakura::logging::get_verbosity_name(message.verbosity),
									 message.message);
		}
	};
} // namespace sakura::editor