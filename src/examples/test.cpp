#include "app.hpp"

int main(int argc, char* argv[]) {
	
	sakura::App app = sakura::App::Builder()
							.set_name("Sakura")
							.set_width(800)
							.set_height(600);

	app.run();
	
	return 0;
}