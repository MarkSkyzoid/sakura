#include "../plugin_system.hpp"
#include "../../platform/platform.hpp"
#include "../../log/log.hpp"

namespace sakura::plugin {
	void PluginDesc::set_load_callback(LoadCallbackType load_callback)
	{
		load_callback_ = load_callback;
	}

	void PluginDesc::set_unload_callback(UnloadCallbackType unload_callback)
	{
		unload_callback_ = unload_callback;
	}

	void PluginDesc::set_shared_library_name(const char* library_name)
	{
		name_ = library_name;
		dynamic_library_.init(library_name);
	}

	void PluginDesc::set_api(APIs api) { plugin_api_ = api; }

	void PluginRegistry::cleanup()
	{
		u32 plugin_index = 0;
		for (auto& plugin_entry : game_plugins_.plugins) {
			PluginHandle handle { plugin_index, APIs::Game };
			remove_plugin(handle);
			plugin_index++;
		}
	}

	PluginHandle PluginRegistry::add_plugin(PluginDesc& plugin_desc)
	{
		PluginHandle out_handle = InvalidPluginHandle;

		// Find the next potential handle
		PluginHandle potential_handle = InvalidPluginHandle;
		switch (plugin_desc.plugin_api_) {
		case APIs::Game: {
			potential_handle.index = static_cast<u32>(game_plugins_.count);
			potential_handle.plugin_api = APIs::Game;
			break;
		}
		case APIs::Count: // Intentional
		default: {
			SKR_ASSERT_M(false, "Plugin API not implemented! It won't be loaded");
			break;
		}
		}

		// If hot reload is enabled this will fill in the load and unload callbacks making the plugin valid!
		poll_plugin(plugin_desc, potential_handle);

		// Try to load the plugin, if for any reasons it fails return an invalid handle to the client.
		SKR_ASSERT_FATAL_M(plugin_desc.load_callback_,
								 "Cannot load plugin as the load callback is nullptr!");

		Payload dummy_payload;
		if (!load_internal(plugin_desc, potential_handle, LoadOptions::FirstLoad, dummy_payload)) {
			return InvalidPluginHandle;
		}

		// Now store the plugin description and increase the count. Also, assign the correct handle
		switch (plugin_desc.plugin_api_) {
		case APIs::Game: {
			out_handle = potential_handle;
			game_plugins_.plugins[game_plugins_.count++].desc = plugin_desc; // store the description
			break;
		}
		case APIs::Count: // Intentional
		default: {
			break;
		}
		}

		return out_handle;
	}

	void PluginRegistry::remove_plugin(const PluginHandle& plugin_handle)
	{
		if (plugin_handle.plugin_api == InvalidPluginHandle.plugin_api ||
			 plugin_handle.index == InvalidPluginHandle.index) {
			SKR_ASSERT_FATAL(false);
		}

		PluginDesc plugin_desc = {};
		switch (plugin_handle.plugin_api) {
		case APIs::Game: {
			plugin_desc = game_plugins_.plugins[plugin_handle.index].desc;
			break;
		}
		case APIs::Count: // Intentional
		default: {
			SKR_ASSERT_M(false, "Plugin API not implemented! It won't be loaded");
			break;
		}
		}

		SKR_ASSERT_FATAL_M(plugin_desc.unload_callback_,
								 "Cannot unload plugin as the unload callback is nullptr!");

		Payload dummy_payload;
		unload_internal(plugin_desc, plugin_handle, LoadOptions::FinalUnload, dummy_payload);
	}

	APIGame& PluginRegistry::query_api(APIGameID api_type, PluginHandle plugin_handle)
	{
		SKR_ASSERT_M(plugin_handle.index != InvalidPluginHandle.index &&
						 plugin_handle.plugin_api != InvalidPluginHandle.plugin_api,
						 "Invalid plugin handle passed!");
		return game_plugins_.plugins[plugin_handle.index].api;
	}

	bool PluginRegistry::load_internal(PluginDesc& plugin_desc, PluginHandle handle, LoadOptions options, const Payload& payload)
	{
		return plugin_desc.load_callback_ ? plugin_desc.load_callback_(*this, options, handle, payload) : false;
	}
	bool PluginRegistry::unload_internal(PluginDesc& plugin_desc, PluginHandle handle, LoadOptions options, Payload& payload)
	{
		return plugin_desc.unload_callback_ ? plugin_desc.unload_callback_(*this, options, handle, payload) : false;
	}

#if !SAKURA_PLUGIN_HOTRELOAD
	void PluginRegistry::poll_plugins() {}

	bool PluginRegistry::poll_plugin(PluginDesc&, PluginHandle) {}

	void PluginDynamicLibrary::init(const char*) {}
#endif //! SAKURA_PLUGIN_HOTRELOAD
} // namespace sakura::plugin