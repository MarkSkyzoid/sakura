#include "scene_browser.hpp"
#include "../ext/imgui/imgui.h"
#include "serialization/serialization.hpp"
#include "ecs/ecs.hpp"

// Custom serialization for ImGui
namespace sakura::ser {
	template<> struct StructVisitor<ImguiSceneWalker>
	{
		inline void visit(ImguiSceneWalker& walker, sakura::ecs::Entity& entity, sakura::ecs::ECS& ecs_instance)
		{
			const bool selected = walker.selected_index == entity.get_index() && entity.is_valid();
			char label[256];
			sprintf_s(label, "Entity %d", entity.get_index());
			if (ImGui::Selectable(label, selected)) {
				walker.selected_index = entity.get_index();
			}
		}

		inline void visit(ImguiSceneWalker& walker, sakura::ecs::ECS& ecs_instance)
		{
			for (sakura::ecs::Entity entity : sakura::ecs::EntityIterator<>(ecs_instance)) {
				visit(walker, entity, ecs_instance);
			}
		}

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
