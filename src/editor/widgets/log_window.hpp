#pragma once

#include "../ext/imgui/imgui.h"

namespace sakura::editor::widgets {
	struct LogWindow
	{
		void clear();
		void add_log(const char* fmt, ...) IM_FMTLIST(2);
		void draw(const char* title, bool* p_opened = NULL);

	private:
		ImGuiTextBuffer Buf;
		ImGuiTextFilter Filter;
		ImVector<int> LineOffsets; // Index to lines offset
		bool ScrollToBottom;
	};
} // namespace sakura::editor::widgets