#pragma once
#include "platform/platform.hpp"
#include "app.hpp"

#ifdef SAKURA_HOTRELOAD_CPP
#undef SAKURA_HOTRELOAD_CPP
#endif / / SAKURA_HOTRELOAD_CPP

#ifdef PLATFORM_WINDOWS
#define SAKURA_HOTRELOAD_CPP 1
#include <Windows.h>
using ModuleType = HMODULE;
#else // PLATFORM_WINDOWS
#define SAKURA_HOTRELOAD_CPP 0
#endif // PLATFORM_WINDOWS

#if SAKURA_HOTRELOAD_CPP
#define SAKURA_HOTRELOAD_CPP_ONLY(x) x
#include <map>
#else // SAKURA_HOTRELOAD_CPP
#define SAKURA_HOTRELOAD_CPP_ONLY(x)
#endif // SAKURA_HOTRELOAD_CPP

// #SK_TODO: Add on_load and on_unload callbacks for dealing with dll states
// #SK_TODO: Wrap all callback set/get/naming logic into a macro (e.g. DEFINE_CALLBACK) as this is hard to handle this way
namespace sakura {
	namespace cpp_hotreloading {
		class HotReloadableModule
		{
		public:
			using InitCleanupCallbackType = AppConfig::InitCleanupCallbackType;
			using NativeMessagePumpCallback = AppConfig::NativeMessagePumpCallback; /* #SK_TODO: DELETE */
			using UpdateCallbackType = AppConfig::UpdateCallbackType;
			using RenderCallbackType = AppConfig::RenderCallbackType;

			inline InitCleanupCallbackType init_callback() { return init_callback_; }
			inline InitCleanupCallbackType cleanup_callback() { return cleanup_callback_; }
			inline UpdateCallbackType fixed_update_callback() { return fixed_update_callback_; }
			inline UpdateCallbackType update_callback() { return update_callback_; }
			inline RenderCallbackType render_callback() { return render_callback_; }
			inline UpdateCallbackType end_of_main_loop_update_callback()
			{
				return end_of_main_loop_update_callback_;
			}
			inline NativeMessagePumpCallback native_message_pump_callback()
			{
				return native_message_pump_callback_;
			}

			inline HotReloadableModule& set_init_callback(InitCleanupCallbackType callback,
																		 const char* SAKURA_HOTRELOAD_CPP_ONLY(callback_name))
			{
				init_callback_ = callback;
				SAKURA_HOTRELOAD_CPP_ONLY(init_callback_name_ = callback_name);
				return *this;
			}
			inline HotReloadableModule& set_cleanup_callback(InitCleanupCallbackType callback,
																			 const char* SAKURA_HOTRELOAD_CPP_ONLY(callback_name))
			{
				cleanup_callback_ = callback;
				SAKURA_HOTRELOAD_CPP_ONLY(cleanup_callback_name_ = callback_name);
				return *this;
			}
			inline HotReloadableModule& set_fixed_update_callback(UpdateCallbackType callback,
																					const char* SAKURA_HOTRELOAD_CPP_ONLY(callback_name))
			{
				fixed_update_callback_ = callback;
				SAKURA_HOTRELOAD_CPP_ONLY(fixed_update_callback_name_ = callback_name);
				return *this;
			}
			inline HotReloadableModule& set_update_callback(UpdateCallbackType callback,
																			const char* SAKURA_HOTRELOAD_CPP_ONLY(callback_name))
			{
				update_callback_ = callback;
				SAKURA_HOTRELOAD_CPP_ONLY(update_callback_name_ = callback_name);
				return *this;
			}
			inline HotReloadableModule& set_render_callback(RenderCallbackType callback,
																			const char* SAKURA_HOTRELOAD_CPP_ONLY(callback_name))
			{
				render_callback_ = callback;
				SAKURA_HOTRELOAD_CPP_ONLY(render_callback_name_ = callback_name);
				return *this;
			}
			inline HotReloadableModule&
			set_end_of_main_loop_update_callback(UpdateCallbackType callback,
															 const char* SAKURA_HOTRELOAD_CPP_ONLY(callback_name))
			{
				end_of_main_loop_update_callback_ = callback;
				SAKURA_HOTRELOAD_CPP_ONLY(end_of_main_loop_update_callback_name_ = callback_name);
				return *this;
			}
			/* #SK_TODO: DELETE */
			inline HotReloadableModule&
			set_native_message_pump_callback(NativeMessagePumpCallback callback,
														const char* SAKURA_HOTRELOAD_CPP_ONLY(callback_name))
			{
				native_message_pump_callback_ = callback;
				SAKURA_HOTRELOAD_CPP_ONLY(native_message_pump_callback_name_ = callback_name);
				return *this;
			}

		private:
			InitCleanupCallbackType init_callback_ = nullptr;
			InitCleanupCallbackType cleanup_callback_ = nullptr;
			UpdateCallbackType fixed_update_callback_ = nullptr;
			UpdateCallbackType update_callback_ = nullptr;
			RenderCallbackType render_callback_ = nullptr;
			UpdateCallbackType end_of_main_loop_update_callback_ = nullptr;
			NativeMessagePumpCallback native_message_pump_callback_ = nullptr;

#if SAKURA_HOTRELOAD_CPP
			friend bool poll_module(HotReloadableModule&, const std::wstring& dll_path);

			const char* init_callback_name_ = nullptr;
			const char* cleanup_callback_name_ = nullptr;
			const char* fixed_update_callback_name_ = nullptr;
			const char* update_callback_name_ = nullptr;
			const char* render_callback_name_ = nullptr;
			const char* end_of_main_loop_update_callback_name_ = nullptr;
			const char* native_message_pump_callback_name_ = nullptr;

			ModuleType module_ = nullptr;
			u64 last_write_ = 0;
#endif // SAKURA_HOTRELOAD_CPP
		};
#if SAKURA_HOTRELOAD_CPP
		bool poll_module(HotReloadableModule& mod, const std::wstring& dll_path);
#else  // SAKURA_HOTRELOAD_CPP
#endif // SAKURA_HOTRELOAD_CPP
	}   // namespace cpp_hotreloading
} // namespace sakura