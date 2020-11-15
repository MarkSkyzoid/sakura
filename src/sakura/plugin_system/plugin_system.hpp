#pragma once
#include "../type_aliases.hpp"
#include "plugin_interfaces.hpp"

#include <array>
#include <string>

#if SAKURA_PLUGIN_HOTRELOAD
#define SAKURA_PLUGIN_HOTRELOAD_ONLY(x) x
#else // SAKURA_PLUGIN_HOTRELOAD
#define SAKURA_PLUGIN_HOTRELOAD_ONLY(x)
#endif // SAKURA_PLUGIN_HOTRELOAD

namespace sakura {
	namespace plugin {
		constexpr size_t MAX_PLUGINS_PER_TYPE = 32;
		constexpr const char* LOAD_CALLBACK_NAME = "load";
		constexpr const char* UNLOAD_CALLBACK_NAME = "unload";
		constexpr const char* PLUGINS_FOLDER = "plugins";

		struct Payload;
		class PluginDesc;
		class PluginRegistry;
		struct PluginHandle;

		enum class LoadOptions : u32
		{
			FirstLoad = 0, // Passed down when the plugin is first initialized
			FinalUnload, // Passed down when the plugin is finally unloaded, not for hot reloading purposes
			LoadForHotReload, // Passed down when the plugin is reloading after unloading for hot reloading (you can deserialize saved state, for example)
			UnloadForHotReload, // Passed down when the plugin is unloading for hot reloading (you can serialize the current state, for example)
			Count
		};
		using LoadCallbackType = bool (*)(PluginRegistry&, LoadOptions, const PluginHandle&, const Payload&);
		using UnloadCallbackType = bool (*)(PluginRegistry&, LoadOptions, const PluginHandle&, Payload&);

		using ModuleType = void*;
		class PluginDynamicLibrary
		{
		public:
			void init(const char* plugin_name);

		private:
#if SAKURA_PLUGIN_HOTRELOAD
			static const size_t FILENAME_LENGTH = 64; 
			char library_filename_[FILENAME_LENGTH];
			ModuleType dynamic_library;
			u64 last_write_ = 0;
#endif // SAKURA_PLUGIN_HOTRELOAD

			friend class PluginRegistry;
		};

		/// <summary>
		/// Use this for any user data you might want to serialize/deserialize for hot reloading
		/// </summary>
		struct Payload
		{
			void* user_data = nullptr;
		};

		class PluginDesc
		{
		public:
			void set_load_callback(LoadCallbackType load_callback);
			void set_unload_callback(UnloadCallbackType unload_callback);
			void set_shared_library_name(const char* library_name);
			void set_api(APIs api);

		private:
			LoadCallbackType load_callback_ = nullptr;
			UnloadCallbackType unload_callback_ = nullptr;
			const char* name_ = "";
			PluginDynamicLibrary dynamic_library_;
			APIs plugin_api_ = APIs::Count;

			friend class PluginRegistry;
		};

		struct PluginHandle
		{
			constexpr PluginHandle() : index(static_cast<u32>(-1)), plugin_api(APIs::Count) {}
			constexpr PluginHandle(u32 i, APIs api) : index(i), plugin_api(api) {}
			u32 index;
			APIs plugin_api;
		};
		constexpr PluginHandle InvalidPluginHandle;

		class PluginRegistry
		{
		public:
			~PluginRegistry() { cleanup(); }

			void cleanup();

			inline void set_plugins_subfolder_name(const char* plugins_subfolder_name)
			{
				plugins_subfolder_name_ = plugins_subfolder_name;
			}
			[[nodiscard]] inline const char* plugins_subfolder_name() const
			{
				return plugins_subfolder_name_;
			}

			PluginHandle add_plugin(PluginDesc& plugin_desc);
			void remove_plugin(const PluginHandle& plugin);

			/// <summary>
			/// Returns the API interface for the given API type, given a plugin handle.
			/// </summary>
			APIGame& query_api(APIGameID api_type, PluginHandle plugin_handle);

			/// <summary>
			/// Use this function at the end of the frame, for hot reloading plugin that have changed (if hot reload is enabled)
			/// </summary>
			void poll_plugins();

		private:
			bool load_internal(PluginDesc& plugin_desc, PluginHandle handle, LoadOptions options, const Payload& payload);
			bool unload_internal(PluginDesc& plugin_desc, PluginHandle handle, LoadOptions options, Payload& payload);
			bool poll_plugin(PluginDesc& plugin_desc, PluginHandle plugin_hand);

#if SAKURA_PLUGIN_HOTRELOAD
			void get_plugin_library_file_path(const PluginDynamicLibrary& dynamic_lib,
														 char* out_path,
														 size_t num_chars_in_out_path) const;
#endif // SAKURA_PLUGIN_HOTRELOAD

			template<typename T> struct PluginArray
			{
				struct Entry
				{
					T api;
					PluginDesc desc;
				};
				std::array<Entry, MAX_PLUGINS_PER_TYPE> plugins;
				size_t count = 0;
			};

			PluginArray<APIGame> game_plugins_;

			const char* plugins_subfolder_name_ = "";
		};
	} // namespace plugin
} // namespace sakura