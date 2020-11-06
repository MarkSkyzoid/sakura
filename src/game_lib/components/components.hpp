#pragma once

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
SAKURA_STRUCT(float2, SAKURA_FIELD(x) SAKURA_FIELD(y));

struct Particle
{
	float2 prev_pos;
	float2 pos;
	float2 vel;

	bool collided = false;

	static Particle make(float2 p, float2 v)
	{
		auto par = Particle { p, p, v };
		return par;
	};
};
SAKURA_STRUCT(Particle, SAKURA_FIELD(pos) SAKURA_FIELD(vel));

struct Wall
{
	float2 n;
	float d;
};
SAKURA_STRUCT(Wall, SAKURA_FIELD(n) SAKURA_FIELD(d));

namespace sakura {
	namespace game_lib {
		namespace components {
			template<typename Visitor>
			void visit(Visitor& visitor, sakura::ecs::ECS& ecs, sakura::ecs::Entity entity)
			{
#define DECLARE_COMPONENT(ComponentType)                     \
	if (ecs.has_component<ComponentType>(entity)) {           \
	visit<Visitor>(visitor, *(ecs.get_component<ComponentType>(entity))); \
	}

				DECLARE_COMPONENT(Particle);
				DECLARE_COMPONENT(Wall);

#undef DECLARE_COMPONENT
			}
		} // namespace components
	}    // namespace game_lib
} // namespace sakura