#include "scene_browser.hpp"

#include "../ext/imgui/imgui.h"
#include "serialization/serialization.hpp"

// Custom serialization for ImGui
namespace sakura::ser {
	inline void visit(ImguiSceneWalker& walker, sakura::ecs::Entity& entity, sakura::ecs::ECS& ecs_instance)
	{
		const bool selected = entity.is_valid() && walker.selected_entity == entity;
		char label[256];
		sprintf_s(label, "Entity %d", entity.get_index());
		if (ImGui::Selectable(label, selected)) {
			walker.selected_entity = entity;
		}
	}

	inline void visit(ImguiSceneWalker& walker, sakura::editor::Scene& scene)
	{
		for (sakura::ecs::Entity entity : sakura::ecs::EntityIterator<>(scene.ecs)) {
			visit(walker, entity, scene.ecs);
		}
	}

	template<> struct StructVisitor<ImguiSceneWalker>
	{
		StructVisitor(const char* name, ImguiSceneWalker& walker) : name_(name), imgui_walker_(walker)
		{}
		~StructVisitor() {}

		template<typename T> StructVisitor& field(const char* label, T& value)
		{
			visit(imgui_walker_, value);
			return *this;
		}

	private:
		const char* name_ = nullptr;
		ImguiSceneWalker& imgui_walker_;
	};
} // namespace sakura::ser

void sakura::editor::widgets::SceneBrowser::draw(sakura::editor::Scene& scene)
{
	ImGui::Begin(name_, &window_open_);
	sakura::ser::visit(ser_imgui_scene_walker_, scene);
	ImGui::End();
}

sakura::ecs::Entity sakura::editor::widgets::SceneBrowser::get_selected_entity() const
{
	return ser_imgui_scene_walker_.selected_entity;
}
