#include "app.hpp"
#include "SDL.h"

#include "log/log.hpp"
#include "platform/input.hpp"

#pragma region DEBUG_DELETE
#include "ecs/ecs.hpp"

#include <array>
#include <algorithm>

SDL_Renderer* g_renderer = nullptr;
static int g_num_balls = 0;

float rand_z_to_o() { return rand() / (RAND_MAX + 1.); }
float rand_mo_to_o() { return rand_z_to_o() * 2.0f - 1.0f; }

int draw_circle(SDL_Renderer* renderer, float x, float y, float radius)
{
	float offsetx, offsety, d;
	float status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLineF(renderer, x - offsety, y + offsetx, x + offsety, y + offsetx);
		status += SDL_RenderDrawLineF(renderer, x - offsetx, y + offsety, x + offsetx, y + offsety);
		status += SDL_RenderDrawLineF(renderer, x - offsetx, y - offsety, x + offsetx, y - offsety);
		status += SDL_RenderDrawLineF(renderer, x - offsety, y - offsetx, x + offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		} else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		} else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}
#pragma endregion DEBUG_DELETE

namespace sakura {
	static PlatformConfig platform_config_from_app_config(const AppConfig& app_config)
	{
		sakura::PlatformConfig out_config;
		out_config.title = app_config.title;
		out_config.width = app_config.width;
		out_config.height = app_config.height;

		return out_config;
	}
} // namespace sakura

void sakura::App::run()
{
	SKR_ASSERT_M(!is_running(), "App %s is already running", config_.title);

	// Init
	init();

	// Run
	Clock main_clock(config_.target_frame_rate);
	main_clock.start_from(0.0);

#pragma region DEBUG_DELETE
	g_renderer = SDL_CreateRenderer((SDL_Window*)platform::get_native_window_handle(platform_), -1,
											  SDL_RENDERER_SOFTWARE);

	std::array<time::Seconds, platform::MAX_KEYS> key_timer {};

	struct float2
	{
		float x, y;
		float2(float x_ = 0, float y_ = 0) : x(x_), y(y_) {};
		float2 operator+(const float2 rhs) const { return float2 { x + rhs.x, y + rhs.y }; }
		float2 operator-(const float2 rhs) const { return float2 { x - rhs.x, y - rhs.y }; }
		float2 operator+(const float rhs) const { return float2 { x + rhs, y + rhs }; }
		float2 operator*(const float rhs) const { return float2 { x * rhs, y * rhs }; }

		float length_sqr() const { return x * x + y * y; }

		float dot(const float2 rhs) const { return (x * rhs.x + y * rhs.y); };
	};

	struct Particle
	{
		float2 pos;
		float2 vel;

		bool collided = false;

		static Particle make(float2 p, float2 v)
		{
			g_num_balls++;
			return Particle { p, v };
		};
	};

	struct Wall
	{
		float2 n;
		float d;
	};

	lecs::ECS ecs {};

	constexpr size_t NUM_PARTICLES = 1;
	constexpr float PARTICLE_RADIUS = 5.0f;
	constexpr float MAX_PARTICLE_VEL = 50.0f;
	constexpr float PARTICLE_ELASTICITY = 1.0f;
	const float2 GRAVITY = float2(0.0f, 1.0f) * 1000.0f; // 10 m/s^2

	// Make particles
	srand(main_clock.time_cycles());
	for (auto i = 0; i < NUM_PARTICLES; i++) {
		auto e = ecs.create_entity();
		ecs.add_component_to_entity<Particle>(e);
		Particle* p = ecs.get_component<Particle>(e);

		auto x = PARTICLE_RADIUS + (config_.width / (float)NUM_PARTICLES) * i;
		*p = Particle::make({ x + rand_z_to_o() * PARTICLE_RADIUS,
									 2.0f * PARTICLE_RADIUS + rand_mo_to_o() * rand_z_to_o() * 0.3f * config_.height },
								  { rand_z_to_o() * MAX_PARTICLE_VEL, rand_z_to_o() * MAX_PARTICLE_VEL });
		p->pos.x = std::max(0.0f, std::min(p->pos.x, config_.width - PARTICLE_RADIUS));
		p->pos.y = std::max(0.0f, std::min(p->pos.y, config_.height - PARTICLE_RADIUS));
	}

	// Make walls
	constexpr size_t NUM_WALLS = 4;
	constexpr auto WALL_OFFSET = PARTICLE_RADIUS + 1;
	Wall walls[NUM_WALLS] = {
		{ float2(0.0f, -1.0f), config_.height - WALL_OFFSET }, // bottom wall
		{ float2(0.0f, 1.0f), WALL_OFFSET },                   // top wall
		{ float2(-1.0f, 0.0f), config_.width - WALL_OFFSET },  // right wall
		{ float2(1.0f, 0.0f), WALL_OFFSET },                   // left wall
	};

	for (auto i = 0; i < NUM_WALLS; i++) {
		auto e = ecs.create_entity();
		ecs.add_component_to_entity<Wall>(e);
		Wall* w = ecs.get_component<Wall>(e);
		*w = walls[i];
	}
#pragma endregion DEBUG_DELETE

	while (!is_exiting_) {
		Duration frame_duration;

		platform::do_message_pump(platform_);

#pragma region DEBUG_DELETE_INPUT_TEST
		const auto& curr_input_state = platform::get_current_input_state(platform_);
		const auto& prev_input_state = platform::get_previous_input_state(platform_);

#if 0
		for (int i = 0; i < platform::KeyboardState::number_of_keys; i++) {
			const auto ideal_frames_number_for_timer = 4.0f * (1.0f / config_.target_frame_rate);
			if (curr_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
				 prev_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
				 key_timer[i] < ideal_frames_number_for_timer) {
				key_timer[i] += main_clock.delta_time_seconds() / main_clock.time_scale();
			} else if (curr_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
						  prev_input_state.keyboard_state.key_states[i] == platform::KeyState::Down) {
				logging::log_info("you are holding the %s key",
										platform::get_name_from_key(static_cast<platform::Key>(i)));
			} else if (curr_input_state.keyboard_state.key_states[i] == platform::KeyState::Down &&
						  prev_input_state.keyboard_state.key_states[i] == platform::KeyState::Up) {

				key_timer[i] = 0.0f;
				logging::log_info("just pressed the %s key",
										platform::get_name_from_key(static_cast<platform::Key>(i)));
			}
		}
#endif
#pragma endregion DEBUG_DELETE_INPUT_TEST

#pragma region DEBUG_DELETE
		{
			// Update
			{
				const auto dt = main_clock.delta_time_seconds();

				// Systems declarations
				auto integration_system = [GRAVITY](ecs::ECS& ecs_instance, float delta_time) {
					for (ecs::Entity entity : ecs::EntityIterator<Particle>(ecs_instance)) {
						Particle* p = ecs_instance.get_component<Particle>(entity);
						p->collided = false; // Reset, hacky
						p->vel = p->vel + GRAVITY * delta_time;
						p->pos = p->pos + p->vel * delta_time;
					}
				};

				auto collision_system = [PARTICLE_RADIUS, PARTICLE_ELASTICITY,
												 MAX_PARTICLE_VEL](ecs::ECS& ecs_instance, float delta_time) {
					auto bounce_off = [](float2 n, float e, float d, Particle& p) -> Particle {
						p.pos = p.pos + n * d;
						p.vel = p.vel - n * p.vel.dot(n) * (1.0f + e);
						return p;
					};

					for (ecs::Entity entity : ecs::EntityIterator<Particle>(ecs_instance)) {
						Particle* pa = ecs_instance.get_component<Particle>(entity);
						Particle& p = *pa;
						if (p.collided)
							continue;

						for (ecs::Entity entity : ecs::EntityIterator<Wall>(ecs_instance)) {
							Wall* w_p = ecs_instance.get_component<Wall>(entity);
							Wall& w = *w_p;
							float vel_dot_norm = p.vel.dot(w.n);
							if (vel_dot_norm < 0.0f) {
								float p_dot_norm = p.pos.dot(w.n);
								if (p_dot_norm + w.d < 0.0f) {
									p = bounce_off(w.n, PARTICLE_ELASTICITY, -(p_dot_norm + w.d), p);

									if (rand_z_to_o() > 0.9) {
										auto e = ecs_instance.create_entity();
										ecs_instance.add_component_to_entity<Particle>(e);
										Particle* new_p = ecs_instance.get_component<Particle>(e);
										*new_p = Particle::make(p.pos, p.vel * -1.0f);
									}
								}
							}
						}

						for (ecs::Entity entity : ecs::EntityIterator<Particle>(ecs_instance)) {
							Particle* pa2 = ecs_instance.get_component<Particle>(entity);
							if (pa2 == pa || pa2->collided)
								continue;

							Particle& p2 = *pa2;
							if (p.vel.dot(p2.vel) < 0.0f) {
								auto p2_to_p = p.pos - p2.pos;
								if (p2_to_p.length_sqr() <= (PARTICLE_RADIUS * PARTICLE_RADIUS)) {
									const float l = sqrt(p2_to_p.length_sqr());
									const float d = PARTICLE_RADIUS - l;
									p = bounce_off(p2_to_p * +(1.0f / (l + 0.0000001)), PARTICLE_ELASTICITY, d, p);
									p2 = bounce_off(p2_to_p * -(1.0f / (l + 0.0000001)), PARTICLE_ELASTICITY, d, p2);
									p.collided = p2.collided = true;
								}
							}
						}
					}
				};

				auto render_system = [PARTICLE_RADIUS](ecs::ECS& ecs_instance, float delta_time,
																	SDL_Renderer* renderer) {
#define SAKURA_RGB 255, 183, 197
					SDL_SetRenderDrawColor(renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef SAKURA_RGB
					SDL_RenderClear(renderer);

					// Render particles
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					for (ecs::Entity entity : ecs::EntityIterator<Particle>(ecs_instance)) {
						Particle* p = ecs_instance.get_component<Particle>(entity);
						draw_circle(renderer, p->pos.x, p->pos.y, PARTICLE_RADIUS);
					}
					SDL_RenderPresent(renderer);
				};

				// Update systems
				integration_system(ecs, dt);

				collision_system(ecs, dt);

				render_system(ecs, dt, g_renderer);
			}

			{
				char title[256];
				sprintf_s(title, "%s - FPS: %f - balls: %d", config_.title,
							 1.0f / main_clock.delta_time_seconds(), g_num_balls);
				platform::set_window_title(platform_, title);
			}
		}
#pragma endregion DEBUG_DELETE

		auto dt_seconds = frame_duration.get();
		main_clock.update(dt_seconds);
	}

#pragma region DEBUG_DELETE
	SDL_DestroyRenderer(g_renderer);
#pragma endregion DEBUG_DELETE

	// Cleanup
	cleanup();
}

void sakura::App::init()
{
	is_running_ = true;

	// Platform system
	auto platform_config = platform_config_from_app_config(config_);
	// This is fine because platform is owned by App. Platform won't outlive App.
	platform_config.exit_callback = [this]() { this->request_exit(); };
	platform_ = platform::create_platform(platform_config);
	platform::init(platform_);
}

void sakura::App::cleanup()
{
	SKR_ASSERT_FAST(platform_);
	platform::cleanup(platform_);
	is_running_ = false;
	is_exiting_ = false;
}

void sakura::App::request_exit() { is_exiting_ = true; }
