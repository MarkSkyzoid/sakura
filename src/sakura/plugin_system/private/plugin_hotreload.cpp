#include "../plugin_system.hpp"
#include "../../platform/platform.hpp"
#if SAKURA_PLUGIN_HOTRELOAD
#ifdef PLATFORM_WINDOWS
#include "../../log/log.hpp"

#include <Windows.h>
#include <codecvt>

// Utilities
static std::wstring string_to_wstring(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstr_to(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr_to[0], size_needed);
	return wstr_to;
}

namespace sakura::plugin {
	constexpr const char* PLUGINS_EXT = "dll";

	void PluginDynamicLibrary::init(const char* plugin_name)
	{
		char path_buffer[256];
		sprintf_s(path_buffer, "%s//%s.%s", PLUGINS_FOLDER, plugin_name, PLUGINS_EXT);
		library_path_ = string_to_wstring(path_buffer);
	}

	bool PluginRegistry::poll_plugin(PluginDesc& plugin_desc, PluginHandle plugin_handle)
	{
		std::wstring dll_path = plugin_desc.dynamic_library_.library_path_;
		bool result = false;
		HANDLE dll_file = CreateFileW(dll_path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (dll_file == INVALID_HANDLE_VALUE) {
			return result;
		}

		auto& mod = plugin_desc.dynamic_library_;

		u64 last_write_time = 0;
		if (GetFileTime(dll_file, NULL, NULL, (FILETIME*)&last_write_time) && mod.last_write_ < last_write_time) {
			if (mod.dynamic_library) {
				FreeLibrary((HMODULE)mod.dynamic_library);
				mod.dynamic_library = NULL;
			}

			// Copy the DLL to avoid holding a lock on the original file
			std::wstring temp_dll_name = dll_path + L"hcpp.dll";
			if (CopyFileW(dll_path.c_str(), temp_dll_name.c_str(), FALSE)) {

				sakura::logging::log_info("reloading plugin \"%s\"...", plugin_desc.name_);

				mod.dynamic_library = LoadLibraryW(temp_dll_name.c_str());
				if (mod.dynamic_library) {
					Payload reload_payload {};
					unload_internal(plugin_desc, plugin_handle, LoadOptions::UnloadForHotReload, reload_payload);

					plugin_desc.set_load_callback(
					(LoadCallbackType)GetProcAddress((HMODULE)mod.dynamic_library, LOAD_CALLBACK_NAME));
					plugin_desc.set_unload_callback(
					(UnloadCallbackType)GetProcAddress((HMODULE)mod.dynamic_library, UNLOAD_CALLBACK_NAME));

					load_internal(plugin_desc, plugin_handle, LoadOptions::LoadForHotReload, reload_payload);

					mod.last_write_ = last_write_time;

					result = plugin_desc.load_callback_ && plugin_desc.unload_callback_;
				}
				sakura::logging::log_info("result: %s", result ? "SUCCESS" : "FAILURE");
			}
		}

		CloseHandle(dll_file);
		return result;
	}

	void PluginRegistry::poll_plugins()
	{
		u32 plugin_index = 0;
		for (auto& plugin_entry : game_plugins_.plugins) {
			PluginHandle handle = { plugin_index, APIs::Game };
			Payload hot_payload;
			poll_plugin(plugin_entry.desc, handle);
			plugin_index++;
		}
	}
} // namespace sakura::plugin

#endif // PLATFORM_WINDOWS
#endif // SAKURA_PLUGIN_HOTRELOAD
