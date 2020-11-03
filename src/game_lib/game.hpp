#pragma once

//#SK_TODO: Make it SDL Independent!!!
#include "SDL.h"
#include "app.hpp"

namespace sakura {
	namespace game_lib {
		void init(const App& app);
		void cleanup(const App& app);
		void fixed_update(f32 dt, const App& app);
		void update(f32 dt, const App& app);
		void render(f32 dt, f32 frame_interpolator, const App& app, SDL_Renderer* renderer);
	}
}