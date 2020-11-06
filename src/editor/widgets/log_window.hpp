#pragma once

#include "../ext/imgui/imgui.h"

namespace sakura::editor::widgets {
	struct LogWindow
	{
		void clear();
		void add_log(const char* fmt, ...) IM_FMTLIST(2);
		void draw(const char* title);

	private:
		ImGuiTextBuffer buf_;
		ImGuiTextFilter filter_;
		ImVector<int> line_offsets_; // Index to lines offset
		bool scroll_to_bottom_;
		bool window_open_ = true;
	};
} // namespace sakura::editor::widgets