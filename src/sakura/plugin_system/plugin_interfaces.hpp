#pragma once
#include "../app.hpp"
#include "../ecs/ecs.hpp"

struct SDL_Renderer;

namespace sakura::plugin {
#define MAKE_CALLBACK(cb_name, cb_type)                                   \
public:                                                                   \
	inline cb_type cb_name##() { return cb_name##_; }                      \
	inline void set_##cb_name(cb_type callback) { cb_name##_ = callback; } \
                                                                          \
	cb_type cb_name##_ = nullptr;

	enum class APIs
	{
		Game = 0,
		Editor,
		Count
	};

	enum class APIGameID
	{
	};

	class APIGame
	{
	public:
		using InitCleanupCallbackType = void (*)(const App& app, sakura::ecs::ECS& ecs_instance);
		using UpdateCallbackType = void (*)(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance);
		using RenderCallbackType = void (*)(f32 dt,
														f32 frame_interpolator,
														const App& app,
														sakura::ecs::ECS& ecs_instance,
														SDL_Renderer* renderer);

		MAKE_CALLBACK(init, InitCleanupCallbackType);
		MAKE_CALLBACK(cleanup, InitCleanupCallbackType);
		MAKE_CALLBACK(fixed_update, UpdateCallbackType);
		MAKE_CALLBACK(update, UpdateCallbackType);
		MAKE_CALLBACK(render, RenderCallbackType);
		MAKE_CALLBACK(end_of_main_loop, UpdateCallbackType);
	};

	class APIEditor
	{
	public:
		using InitCleanupCallbackType = void (*)(const App& app, sakura::ecs::ECS& ecs_instance);
		using UpdateImguiCallbackType = void (*)(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance, void* imgui_context);

		MAKE_CALLBACK(init, InitCleanupCallbackType);
		MAKE_CALLBACK(cleanup, InitCleanupCallbackType);
		MAKE_CALLBACK(update_imgui, UpdateImguiCallbackType);
	};

	enum class APIEditorID
	{
	};

#undef MAKE_CALLBACK
} // namespace sakura::plugin