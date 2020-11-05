#pragma once

#include "ecs/ecs.hpp"
#include "serialization/serialization.hpp"

namespace sakura::editor {
	struct Scene
	{
		sakura::ecs::ECS ecs;
	};
} // namespace sakura::editor
SAKURA_STRUCT(sakura::editor::Scene, SAKURA_FIELD(ecs));