#pragma once
#include "../ecs/ecs.hpp"

namespace sakura::components {
	struct Tag
	{
		std::string name;
	};

	template<typename Visitor>
	void visit_components(Visitor& visitor, sakura::ecs::ECS& ecs, sakura::ecs::Entity entity)
	{
#define DECLARE_COMPONENT(ComponentType)                                                 \
	if (ecs.has_component<ComponentType>(entity)) {                                       \
		sakura::ser::visit<Visitor>(visitor, *(ecs.get_component<ComponentType>(entity))); \
	}

		DECLARE_COMPONENT(sakura::components::Tag);

#undef DECLARE_COMPONENT
	}
} // namespace sakura::components

SAKURA_STRUCT(sakura::components::Tag, SAKURA_FIELD(name));
