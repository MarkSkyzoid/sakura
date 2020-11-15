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

static void create_directory_recursive(wchar_t* path) {
	wchar_t folder[MAX_PATH];
	wchar_t* end;
	ZeroMemory(folder, MAX_PATH * sizeof(wchar_t));

	end = wcschr(path, L'/');

	while (end != NULL) {
		wcsncpy_s(folder, MAX_PATH, path, end - path + 1);
		if (!CreateDirectoryW(folder, NULL)) {
			DWORD err = GetLastError();

			if (err != ERROR_ALREADY_EXISTS) {
				// do whatever handling you'd like
			}
		}
		end = wcschr(++end, L'/');
	}
}

namespace sakura::plugin {
	constexpr const char* PLUGINS_EXT = "dll";

	void PluginDynamicLibrary::init(const char* plugin_name)
	{
		sprintf_s(library_filename_, FILENAME_LENGTH, "%s.%s", plugin_name, PLUGINS_EXT);
	}

	void PluginRegistry::get_plugin_library_file_path(const PluginDynamicLibrary& dynamic_lib,
																	  char* out_path,
																	  size_t num_chars_in_out_path) const
	{
		sprintf_s(out_path, num_chars_in_out_path, "%s//%s//%s", PLUGINS_FOLDER,
					 plugins_subfolder_name(), dynamic_lib.library_filename_);
	}

	bool PluginRegistry::poll_plugin(PluginDesc& plugin_desc, PluginHandle plugin_handle)
	{
		char dll_path_buffer[256];
		get_plugin_library_file_path(plugin_desc.dynamic_library_, dll_path_buffer, 256);
		std::wstring dll_path = string_to_wstring(dll_path_buffer);
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

			// Copy the DLL to a temportary folder in order toavoid holding a lock on the original file
			std::wstring temp_dll_path = L"temp//" + string_to_wstring(PLUGINS_FOLDER) + L"//" +
												  string_to_wstring(plugins_subfolder_name()) + L"//";
			std::wstring temp_dll_name = L"temp//" + dll_path + L"hcpp.dll";
			// We need to manually create the temporary folders one by one on windows.
			create_directory_recursive(temp_dll_path.data());

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
