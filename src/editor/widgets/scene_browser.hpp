#pragma once
#include "../scene/scene.hpp"

namespace sakura::ser {
	struct ImguiSceneWalker
	{
		sakura::ecs::EntityIndex selected_index = sakura::ecs::Entity::INVALID_INDEX;
	};
} // namespace sakura::ser

namespace sakura::editor::widgets {
	class SceneBrowser
	{
	public:
		void draw(sakura::editor::Scene& scene);

		void set_name(const char* name) { name_ = name; }
		const char* get_name() const { return name_; }

	private:
		bool window_open_ = true;
		const char* name_ = "Scene";
		sakura::ser::ImguiSceneWalker ser_imgui_scene_walker_;
	};

} // namespace sakura::editor::widgets