#include "../cpp_hotreloading.hpp"

bool validate_module(sakura::cpp_hotreloading::HotReloadableModule& mod)
{
	return mod.init_callback() && mod.cleanup_callback() && mod.fixed_update_callback() &&
			 mod.update_callback() && mod.render_callback() &&
			 mod.end_of_main_loop_update_callback() && mod.native_message_pump_callback();
}
#if SAKURA_HOTRELOAD_CPP && PLATFORM_WINDOWS
bool sakura::cpp_hotreloading::poll_module(HotReloadableModule& mod, const std::wstring& dll_path)
{
	using namespace sakura::cpp_hotreloading;
	bool result = false;
	HANDLE dll_file = CreateFileW(dll_path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
											NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (dll_file == INVALID_HANDLE_VALUE) {
		return result;
	}

	u64 last_write_time = 0;
	if (GetFileTime(dll_file, NULL, NULL, (FILETIME*)&last_write_time) && mod.last_write_ < last_write_time) {
		if (mod.module_) {
			FreeLibrary((HMODULE)mod.module_);
			mod.module_ = NULL;
		}

		// Copy the DLL to avoid holding a lock on the original file
		std::wstring temp_dll_name = dll_path + L"hcpp.dll";
		if (CopyFileW(dll_path.c_str(), temp_dll_name.c_str(), FALSE)) {
			mod.module_ = LoadLibraryW(temp_dll_name.c_str());
			if (mod.module_) {
				mod
				.set_init_callback((HotReloadableModule::InitCleanupCallbackType)
										 GetProcAddress((HMODULE)mod.module_, mod.init_callback_name_),
										 mod.init_callback_name_)
				.set_cleanup_callback((HotReloadableModule::InitCleanupCallbackType)
											 GetProcAddress((HMODULE)mod.module_, mod.cleanup_callback_name_),
											 mod.cleanup_callback_name_)
				.set_fixed_update_callback((HotReloadableModule::UpdateCallbackType)
													GetProcAddress((HMODULE)mod.module_, mod.fixed_update_callback_name_),
													mod.fixed_update_callback_name_)
				.set_update_callback((HotReloadableModule::UpdateCallbackType)
											GetProcAddress((HMODULE)mod.module_, mod.update_callback_name_),
											mod.update_callback_name_)
				.set_render_callback((HotReloadableModule::RenderCallbackType)
											GetProcAddress((HMODULE)mod.module_, mod.render_callback_name_),
											mod.render_callback_name_)
				.set_end_of_main_loop_update_callback((HotReloadableModule::UpdateCallbackType)
																  GetProcAddress((HMODULE)mod.module_, mod.end_of_main_loop_update_callback_name_),
																  mod.end_of_main_loop_update_callback_name_)
				.set_native_message_pump_callback((HotReloadableModule::NativeMessagePumpCallback)
															 GetProcAddress((HMODULE)mod.module_, mod.native_message_pump_callback_name_),
															 mod.native_message_pump_callback_name_);

				mod.last_write_ = last_write_time;
				result = validate_module(mod);
			}
		}
	}

	CloseHandle(dll_file);
	return result;
}
#else // SAKURA_HOTRELOAD_CPP && PLATFORM_WINDOWS
#pragma message("Unimplemented cpp_hotreloading for platform!")
#endif // SAKURA_HOTRELOAD_CPP && PLATFORM_WINDOWS
