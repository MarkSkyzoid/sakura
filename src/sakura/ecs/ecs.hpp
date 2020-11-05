#pragma once

// #SK_TODO: Config lecs
#include "../../ext/lecs/lecs/lecs.hpp"
#include "serialization/serialization.hpp"

namespace sakura {
	namespace ecs {
		using ECS = lecs::ECS;
		using Entity = lecs::Entity;
		using EntityIndex = lecs::EntityIndex;
		using EntityGeneration = lecs::EntityGeneration;
		using ComponentMask = lecs::ComponentMask;

		template<typename... ComponentTypes>
		using EntityIterator = lecs::EntityIterator<ComponentTypes...>;
	} // namespace ecs
} // namespace sakura

SAKURA_STRUCT(sakura::ecs::ECS, );