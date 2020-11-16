#include "../editor_ui_plugin.hpp"

#include "../../../../sakura/app.hpp"
#include "../../../../sakura/log/log.hpp"
#include "../../../../sakura/type_aliases.hpp"

#include "../../../../ext/imgui/imgui.h"
#include "../../../../ext/IconFontCppHeaders/IconsFontAwesome5.h"

#include "SDL.h"

namespace editor_ui_plugin {
	using namespace sakura::plugin;
	using namespace sakura;

	static void init(const App& app, sakura::ecs::ECS& ecs_instance) {}
	static void cleanup(const App& app, sakura::ecs::ECS& ecs_instance) {}
	static void update_imgui(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance, void* imgui_context)
	{
		ImGui::SetCurrentContext((ImGuiContext*)imgui_context);

		static bool b_editor_plugin_window_open = true;
		if (ImGui::Begin(ICON_FA_PLUG "Plugin Window###PluginWindow", &b_editor_plugin_window_open)) {
			ImGui::Text("This is a window that can be hot reloaded");
			//ImGui::Text("This is a window that HAS BEEN HOT RELOADED");
			ImGui::End();
		}
	}

	EDITOR_UI_PLUGIN_API
	bool load(PluginRegistry& plugin_registry, LoadOptions load_options, const PluginHandle& plugin_handle, const Payload&)
	{
		APIEditor& editor_api = plugin_registry.query_api(APIEditorID {}, plugin_handle);
		editor_api.set_init(editor_ui_plugin::init);
		editor_api.set_cleanup(editor_ui_plugin::cleanup);
		editor_api.set_update_imgui(editor_ui_plugin::update_imgui);
		return true;
	}

	EDITOR_UI_PLUGIN_API
	bool unload(PluginRegistry&, LoadOptions, const PluginHandle&, Payload&) { return true; }
} // namespace editor_ui_plugin
