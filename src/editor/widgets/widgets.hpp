#pragma once
#include "log/log.hpp"

#include "entity_inspector.hpp"
#include "log_window.hpp"
#include "scene_browser.hpp"
#include "toolbar.hpp"

namespace sakura::editor {
	struct Widgets
	{
		widgets::LogWindow log_window;
		widgets::Toolbar toolbar;
		widgets::SceneBrowser scene_browser;
		widgets::EntityInspector entity_inspector;

		static void log_callback(void* user_data, const sakura::logging::Message& message)
		{
			using namespace widgets;
			LogWindow* log_window = static_cast<LogWindow*>(user_data);
			log_window->add_log("[%s] %s\n", sakura::logging::get_verbosity_name(message.verbosity),
									 message.message);
		}
	};
} // namespace sakura::editor