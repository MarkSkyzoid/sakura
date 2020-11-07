#pragma once
#include "type_aliases.hpp"

namespace sakura {
	class ImGuiLayer
	{
	public:
		ImGuiLayer() = default;
		~ImGuiLayer() = default;

		void init(void* native_window_handle, sakura::f32 width, sakura::f32 height);
		void cleanup();

		bool visible() const { return visible_; }
		void set_visible(bool visible) { visible_ = visible; }
		bool interactive() const { return interactive_; }
		void set_interactive(bool interactive) { interactive_ = interactive; }

		f32 width() const { return width_; }
		f32 height() const { return height_; }

		void new_frame(void* native_window_handle);
		void render();
		void on_event(const void* event_data_ptr);

	private:
		bool visible_ = true;
		bool interactive_ = true;
		f32 width_;
		f32 height_;
	};
} // namespace sakura