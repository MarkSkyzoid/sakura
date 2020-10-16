#include "app.hpp"

// #SK_TODO: Create proper platform layer - https://github.com/MarkSkyzoid/sakura/issues/2
#include "SDL.h" 
#include "log.hpp"

void sakura::App::run()
{
	SKR_ASSERT_M(!is_running(), "App %s is already running!", config_.name);

	// Init
	is_running_ = true;

	// #SK_TODO: Create proper platform layer - https://github.com/MarkSkyzoid/sakura/issues/2
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow(
		config_.name,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		config_.width,
		config_.height,
		0
	);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	// Run
	bool is_exiting = false;
	while (!is_exiting) {

		// #SK_TODO: Create proper platform layer - https://github.com/MarkSkyzoid/sakura/issues/2
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				is_exiting = true;
			}
		}
#define SAKURA_RGB 255, 183, 197
		SDL_SetRenderDrawColor(renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef  SAKURA_RGB
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	// Cleanup

	// #SK_TODO: Create proper platform layer - https://github.com/MarkSkyzoid/sakura/issues/2
	SDL_DestroyWindow(window);
	SDL_Quit();
}
