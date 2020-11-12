#pragma once
#include "../sakura/plugin_system/plugin_system.hpp"

#if SAKURA_PLUGIN_HOTRELOAD
#ifdef TEST_PLUGIN_EXPORTS
#define TEST_PLUGIN_API extern "C" __declspec(dllexport)
#else
#define TEST_PLUGIN_API extern "C" __declspec(dllimport)
#endif // TEST_PLUGIN_EXPORTS
#else
#define TEST_PLUGIN_API
#endif // SAKURA_PLUGIN_HOTRELOAD

#define TEST_PLUGIN_NAME "test_plugin"

namespace test_plugin {
	using namespace sakura::plugin;
	TEST_PLUGIN_API bool load(PluginRegistry&, LoadOptions, const PluginHandle&, const Payload&);
	TEST_PLUGIN_API bool unload(PluginRegistry&, LoadOptions, const PluginHandle&, Payload&);
	inline PluginDesc create_plugin() {
		PluginDesc desc;
		desc.set_api(sakura::plugin::APIs::Game);
		desc.set_shared_library_name(TEST_PLUGIN_NAME);

#if SAKURA_PLUGIN_HOTRELOAD
		desc.set_load_callback(nullptr);
		desc.set_unload_callback(nullptr);
#else  // SAKURA_PLUGIN_HOTRELOAD
		desc.set_load_callback(test_plugin::load);
		desc.set_unload_callback(test_plugin::unload);
#endif // SAKURA_PLUGIN_HOTRELOAD

		return desc;
	}
}