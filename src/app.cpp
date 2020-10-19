#include "app.hpp"

// #SK_TODO: Create proper platform layer - https://github.com/MarkSkyzoid/sakura/issues/2
#include "SDL.h" 
#include "log.hpp"

static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;

void sakura::App::run()
{
	SKR_ASSERT_M(!is_running(), "App %s is already running", config_.name);

	// Init
	init();

	// Run
	bool is_exiting = false;
	while (!is_exiting) {
		// #SK_TODO: Create platform layer message pump abstraction - https://github.com/MarkSkyzoid/sakura/issues/4
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				is_exiting = true;
			}
		}
#define SAKURA_RGB 255, 183, 197
		SDL_SetRenderDrawColor(g_renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef  SAKURA_RGB
		SDL_RenderClear(g_renderer);
		SDL_RenderPresent(g_renderer);
	}

	// Cleanup
	cleanup();
}

void sakura::App::init()
{
	is_running_ = true;

	// #SK_TODO: Create proper platform layer - https://github.com/MarkSkyzoid/sakura/issues/2
	SDL_Init(SDL_INIT_VIDEO);
	// #SK_TODO: Create platform window abstraction - https://github.com/MarkSkyzoid/sakura/issues/3
	g_window = SDL_CreateWindow(
		config_.name,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		config_.width,
		config_.height,
		0
	);
	g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_SOFTWARE);
}

void sakura::App::cleanup()
{
	// #SK_TODO: Create proper platform layer - https://github.com/MarkSkyzoid/sakura/issues/2
	SDL_DestroyWindow(g_window);
	SDL_Quit();
}
