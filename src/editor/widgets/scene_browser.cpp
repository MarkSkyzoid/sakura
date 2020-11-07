#include "scene_browser.hpp"

#include "../ext/imgui/imgui.h"
#include "serialization/serialization.hpp"
#include "components/components.hpp"
#include "IconsFontAwesome5.h"

// Custom serialization for ImGui
namespace sakura::ser {
	inline void visit(ImguiSceneWalker& walker, sakura::ecs::Entity& entity, sakura::ecs::ECS& ecs_instance)
	{
		const bool selected = entity.is_valid() && walker.selected_entity == entity;
		char label[256];
		if (ecs_instance.has_component<sakura::components::Tag>(entity)) {
			sakura::components::Tag* tag = ecs_instance.get_component<sakura::components::Tag>(entity);
			sprintf_s(label, "%s", tag->name.c_str());
		} else {
			sprintf_s(label, "Entity %d", entity.get_index());
		}

		if (walker.filter.PassFilter(label)) {
			sprintf_s(label, "%s###Entity%d", label, entity.get_index());

			if (ImGui::Selectable(label, selected)) {
				walker.selected_entity = entity;
			}
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

void sakura::editor::widgets::SceneBrowser::draw(const char* title, sakura::editor::Scene& scene)
{
	ImGui::Begin(title, &window_open_);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - ImGui::GetFontSize());
	filter.Draw(ICON_FA_SEARCH "###filter");
	ImGui::Separator();
	ser_imgui_scene_walker_.filter = filter;
	sakura::ser::visit(ser_imgui_scene_walker_, scene);
	ImGui::End();
}

sakura::ecs::Entity sakura::editor::widgets::SceneBrowser::get_selected_entity() const
{
	return ser_imgui_scene_walker_.selected_entity;
}
