#pragma once

struct ImGuiViewport;
namespace sakura::editor::widgets {
	struct Toolbar
	{
		using ButtonPressCallback = void (*)();
		float size = 40.0f;
		void draw(ImGuiViewport* viewport, float menu_bar_height);

		ButtonPressCallback play_press_callback = nullptr;
		ButtonPressCallback pause_press_callback = nullptr;
		ButtonPressCallback stop_press_callback = nullptr;

	private:
		void add_buttons();
	};
} // namespace sakura::editor::widgets