#pragma once

// #SK_TODO: Config lecs
#include "../../ext/lecs/lecs/lecs.hpp"

namespace sakura {
	namespace ecs {
		using ECS = lecs::ECS;
		using Entity = lecs::Entity;
		using ComponentMask = lecs::ComponentMask;

		template<typename... ComponentTypes>
		using EntityIterator = lecs::EntityIterator<ComponentTypes...>;
	} // namespace ecs
} // namespace sakura