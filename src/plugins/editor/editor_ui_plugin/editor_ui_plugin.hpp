// This will be a template from which to generate all other plugins.
// #SK_TODO: through an automatic tool!

#pragma once
#include "../../../sakura/plugin_system/plugin_system.hpp"

#if SAKURA_PLUGIN_HOTRELOAD
#ifdef EDITOR_UI_PLUGIN_EXPORTS
#define EDITOR_UI_PLUGIN_API extern "C" __declspec(dllexport)
#else
#define EDITOR_UI_PLUGIN_API extern "C" __declspec(dllimport)
#endif // EDITOR_UI_PLUGIN_EXPORTS
#else
#define EDITOR_UI_PLUGIN_API
#endif // SAKURA_PLUGIN_HOTRELOAD

#define EDITOR_UI_PLUGIN_NAME "editor_ui_plugin"

namespace editor_ui_plugin {
	using namespace sakura::plugin;
	EDITOR_UI_PLUGIN_API bool load(PluginRegistry&, LoadOptions, const PluginHandle&, const Payload&);
	EDITOR_UI_PLUGIN_API bool unload(PluginRegistry&, LoadOptions, const PluginHandle&, Payload&);
	inline PluginDesc create_plugin()
	{
		PluginDesc desc;
		desc.set_api(sakura::plugin::APIs::Game);
		desc.set_shared_library_name(EDITOR_UI_PLUGIN_NAME);

#if SAKURA_PLUGIN_HOTRELOAD
		desc.set_load_callback(nullptr);
		desc.set_unload_callback(nullptr);
#else  // SAKURA_PLUGIN_HOTRELOAD
		desc.set_load_callback(test_plugin::load);
		desc.set_unload_callback(test_plugin::unload);
#endif // SAKURA_PLUGIN_HOTRELOAD

		return desc;
	}
} // namespace test_plugin