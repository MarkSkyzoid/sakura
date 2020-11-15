#include "../editor_ui_plugin.hpp"

#include "../../../../sakura/app.hpp"
#include "../../../../sakura/log/log.hpp"
#include "../../../../sakura/type_aliases.hpp"

#include "SDL.h"

namespace editor_ui_plugin {
	using namespace sakura::plugin;
	using namespace sakura;

	static void init(const App& app, sakura::ecs::ECS& ecs_instance) {}
	static void cleanup(const App& app, sakura::ecs::ECS& ecs_instance) {}
	static void fixed_update(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance) {}
	static void update(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance) {}
	static void render(f32 dt, f32 frame_interpolator, const App& app, sakura::ecs::ECS& ecs_instance, SDL_Renderer* renderer)
	{}

	EDITOR_UI_PLUGIN_API
	bool load(PluginRegistry& plugin_registry, LoadOptions load_options, const PluginHandle& plugin_handle, const Payload&)
	{
		APIGame& game_api = plugin_registry.query_api(APIGameID::ID, plugin_handle);
		game_api.set_init(editor_ui_plugin::init);
		game_api.set_cleanup(editor_ui_plugin::cleanup);
		game_api.set_fixed_update(editor_ui_plugin::fixed_update);
		game_api.set_update(editor_ui_plugin::update);
		game_api.set_render(editor_ui_plugin::render);
		return true;
	}

	EDITOR_UI_PLUGIN_API
	bool unload(PluginRegistry&, LoadOptions, const PluginHandle&, Payload&) { return true; }
} // namespace editor_ui_plugin
