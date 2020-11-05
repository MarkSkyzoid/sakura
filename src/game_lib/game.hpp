#pragma once

//#SK_TODO: Make it SDL Independent!!!
#include "SDL.h"
#include "app.hpp"
#include "ecs/ecs.hpp"

namespace sakura {
	namespace game_lib {
		void init(const App& app, sakura::ecs::ECS& ecs_instance);
		void cleanup(const App& app, sakura::ecs::ECS& ecs_instance);
		void fixed_update(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance);
		void update(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance);
		void render(f32 dt, f32 frame_interpolator, const App& app, sakura::ecs::ECS& ecs_instance, SDL_Renderer* renderer);
	} // namespace game_lib
} // namespace sakura