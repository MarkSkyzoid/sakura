#include "game.hpp"
#include "log/log.hpp"
#include "serialization/serialization.hpp"
#include "SDL.h"

#include "ecs/ecs.hpp"

#include <array>
#include <algorithm>
#include <sstream>

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
SAKURA_STRUCT(float2, SAKURA_FIELD(x), SAKURA_FIELD(y));

struct Particle
{
	float2 prev_pos;
	float2 pos;
	float2 vel;

	bool collided = false;

	static Particle make(float2 p, float2 v)
	{
		g_num_balls++;
		auto par = Particle { p, p, v };
		std::stringstream ss;
		sakura::ser::print_struct(ss, par);
		sakura::logging::log_info(ss.str().c_str());
		return par;
	};
};
SAKURA_STRUCT(Particle, SAKURA_FIELD(pos) SAKURA_FIELD(vel));

struct Wall
{
	float2 n;
	float d;
};

constexpr size_t NUM_PARTICLES = 1;
constexpr float PARTICLE_RADIUS = 5.0f;
constexpr float MAX_PARTICLE_VEL = 50.0f;
constexpr float PARTICLE_ELASTICITY = 1.0f;
const float2 GRAVITY = float2(0.0f, 1.0f) * 1000.0f; // 10 m/s^2

void sakura::game_lib::init(const App& app, sakura::ecs::ECS& ecs_instance)
{
	const auto WIDTH = app.config().width;
	const auto HEIGHT = app.config().height;

	// Make particles
	srand(app.main_clock().time_cycles());
	for (auto i = 0; i < NUM_PARTICLES; i++) {
		auto e = ecs_instance.create_entity();
		ecs_instance.add_component_to_entity<Particle>(e);
		Particle* p = ecs_instance.get_component<Particle>(e);

		auto x = PARTICLE_RADIUS + (WIDTH / (float)NUM_PARTICLES) * i;
		*p = Particle::make({ x + rand_z_to_o() * PARTICLE_RADIUS,
									 2.0f * PARTICLE_RADIUS + rand_mo_to_o() * rand_z_to_o() * 0.3f * HEIGHT },
								  { rand_z_to_o() * MAX_PARTICLE_VEL, rand_z_to_o() * MAX_PARTICLE_VEL });
		p->pos.x = std::max(0.0f, std::min(p->pos.x, WIDTH - PARTICLE_RADIUS));
		p->pos.y = std::max(0.0f, std::min(p->pos.y, HEIGHT - PARTICLE_RADIUS));
	}

	// Make walls
	constexpr size_t NUM_WALLS = 4;
	constexpr auto WALL_OFFSET = PARTICLE_RADIUS + 1;
	Wall walls[NUM_WALLS] = {
		{ float2(0.0f, -1.0f), HEIGHT - WALL_OFFSET }, // bottom wall
		{ float2(0.0f, 1.0f), WALL_OFFSET },           // top wall
		{ float2(-1.0f, 0.0f), WIDTH - WALL_OFFSET },  // right wall
		{ float2(1.0f, 0.0f), WALL_OFFSET },           // left wall
	};

	for (auto i = 0; i < NUM_WALLS; i++) {
		auto e = ecs_instance.create_entity();
		ecs_instance.add_component_to_entity<Wall>(e);
		Wall* w = ecs_instance.get_component<Wall>(e);
		*w = walls[i];
	}
}

void sakura::game_lib::cleanup(const App& app, sakura::ecs::ECS& ecs_instance) {}

void sakura::game_lib::fixed_update(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance)
{
	auto integration_system = [](sakura::ecs::ECS& ecs_instance, float delta_time) {
		for (sakura::ecs::Entity entity : sakura::ecs::EntityIterator<Particle>(ecs_instance)) {
			Particle* p = ecs_instance.get_component<Particle>(entity);
			p->prev_pos = p->pos;
			p->collided = false; // Reset, hack;
			p->vel = p->vel + GRAVITY * delta_time;
			p->pos = p->pos + p->vel * delta_time;
		}
	};

	auto collision_system = [](sakura::ecs::ECS& ecs_instance, float delta_time) {
		auto bounce_off = [](float2 n, float e, float d, Particle& p) -> Particle {
			p.pos = p.pos + n * d;
			p.vel = p.vel - n * p.vel.dot(n) * (1.0f + e);
			return p;
		};

		for (sakura::ecs::Entity entity : sakura::ecs::EntityIterator<Particle>(ecs_instance)) {
			Particle* pa = ecs_instance.get_component<Particle>(entity);
			Particle& p = *pa;
			if (p.collided)
				continue;

			for (sakura::ecs::Entity entity : sakura::ecs::EntityIterator<Wall>(ecs_instance)) {
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

			for (sakura::ecs::Entity entity : sakura::ecs::EntityIterator<Particle>(ecs_instance)) {
				Particle* pa2 = ecs_instance.get_component<Particle>(entity);
				if (pa2 == pa || pa2->collided)
					continue;

				Particle& p2 = *pa2;
				if (p.vel.dot(p2.vel) < 0.0f) {
					auto p2_to_p = p.pos - p2.pos;
					if (p2_to_p.length_sqr() <= (2.0f * PARTICLE_RADIUS) * (2.0f * PARTICLE_RADIUS)) {
						const float l = sqrt(p2_to_p.length_sqr());
						const float d = PARTICLE_RADIUS + PARTICLE_RADIUS - l;
						p = bounce_off(p2_to_p * (1.0f / (l + 0.0000001)), PARTICLE_ELASTICITY, d, p);
						p2 = bounce_off(p2_to_p * (1.0f / (l + 0.0000001)), PARTICLE_ELASTICITY, -d, p2);
						p.collided = p2.collided = true;
					}
				}
			}
		}
	};

	integration_system(ecs_instance, dt);
	collision_system(ecs_instance, dt);
}

void sakura::game_lib::update(f32 dt, const App& app, sakura::ecs::ECS& ecs_instance) {}

void sakura::game_lib::render(f32 dt, f32 frame_interpolator, const App& app, sakura::ecs::ECS& ecs_instance, SDL_Renderer* renderer)
{
	auto render_system = [](sakura::ecs::ECS& ecs_instance, float delta_time, float interpolator,
									SDL_Renderer* renderer) {
#define SAKURA_RGB 255, 183, 197
		SDL_SetRenderDrawColor(renderer, SAKURA_RGB, SDL_ALPHA_OPAQUE);
#undef SAKURA_RGB
		SDL_RenderClear(renderer);

		// Render particles
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		for (sakura::ecs::Entity entity : sakura::ecs::EntityIterator<Particle>(ecs_instance)) {
			Particle* p = ecs_instance.get_component<Particle>(entity);
			auto x = p->pos.x * interpolator + p->prev_pos.x * (1.0f - interpolator);
			auto y = p->pos.y * interpolator + p->prev_pos.y * (1.0f - interpolator);
			draw_circle(renderer, x, y, PARTICLE_RADIUS);
		}
		SDL_RenderPresent(renderer);
	};

	render_system(ecs_instance, dt, frame_interpolator, renderer);
}
