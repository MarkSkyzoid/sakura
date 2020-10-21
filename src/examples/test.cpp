#include "app.hpp"

int main(int argc, char* argv[]) 
{
	
	sakura::App app = sakura::App::Builder()
							.set_name("Sakura")
							.set_width(800)
							.set_height(600)
							.set_target_frame_rate(30.0f);

	app.run();
	
	return 0;
}