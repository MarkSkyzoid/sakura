#include "entity_inspector.hpp"

#include "serialization/serialization.hpp"


void sakura::editor::widgets::EntityInspector::draw(sakura::editor::Scene& scene,
																	 const sakura::ecs::Entity& entity,
																	 sakura::ser::ImGuiEntityInspectorWalker::VisitComponentsFn visit_components_callback)
{
	ImGui::Begin(name_, &window_open_);
	sakura::ser::ImGuiEntityInspectorWalker walker { entity, name_, visit_components_callback };
	visit(walker, scene);
	ImGui::End();
}
