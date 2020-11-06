#include "entity_inspector.hpp"

#include "serialization/serialization.hpp"

void sakura::editor::widgets::EntityInspector::draw(const char* title,
																	 sakura::editor::Scene& scene,
																	 const sakura::ecs::Entity& entity,
																	 sakura::ser::ImGuiEntityInspectorWalker::VisitComponentsFn visit_components_callback)
{
	ImGui::Begin(title, &window_open_);
	sakura::ser::ImGuiEntityInspectorWalker walker { entity, title, visit_components_callback };
	visit(walker, scene);
	ImGui::End();
}
