#pragma once
#include "../scene/scene.hpp"
#include "imgui.h"

namespace sakura::ser {
	struct ImguiSceneWalker
	{
		ImGuiTextFilter filter;
		sakura::ecs::Entity selected_entity = sakura::ecs::Entity::Invalid;
	};
} // namespace sakura::ser

namespace sakura::editor::widgets {
	class SceneBrowser
	{
	public:
		void draw(const char* title, sakura::editor::Scene& scene);
		sakura::ecs::Entity get_selected_entity() const;

	private:
		bool window_open_ = true;
		sakura::ser::ImguiSceneWalker ser_imgui_scene_walker_;
		ImGuiTextFilter filter;
	};

} // namespace sakura::editor::widgets