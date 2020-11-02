#include "app.hpp"
#include "log/log.hpp"

void fixed_update(sakura::f32 dt) { sakura::logging::log_info("fixed_update(%f)", dt); }
void update(sakura::f32 dt) { sakura::logging::log_info("update(%f)", dt); }
void render(sakura::f32 dt, sakura::f32 frame_interpolator)
{
	sakura::logging::log_info("render(%f, %f)", dt, frame_interpolator);
}

int main(int argc, char* argv[])
{

	sakura::App app = sakura::App::Builder()
							.set_title("Sakura")
							.set_width(800)
							.set_height(600)
							.set_target_frame_rate(60.0f)
							.set_fixed_update_callback(fixed_update)
							.set_update_callback(update)
							.set_render_callback(render);

	app.run();

	return 0;
}