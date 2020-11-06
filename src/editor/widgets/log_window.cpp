#include "log_window.hpp"

namespace sakura::editor::widgets {

	void LogWindow::clear()
	{
		buf_.clear();
		line_offsets_.clear();
	}

	void LogWindow::add_log(const char* fmt, ...) IM_FMTLIST(2)
	{
		int old_size = buf_.size();
		va_list args;
		va_start(args, fmt);
		buf_.appendfv(fmt, args);
		va_end(args);
		for (int new_size = buf_.size(); old_size < new_size; old_size++)
			if (buf_[old_size] == '\n')
				line_offsets_.push_back(old_size);
		scroll_to_bottom_ = true;
	}

	void LogWindow::draw(const char* title)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin(title, &window_open_);
		if (ImGui::Button("Clear"))
			clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		filter_.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
		if (copy)
			ImGui::LogToClipboard();

		if (filter_.IsActive()) {
			const char* buf_begin = buf_.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != NULL; line_no++) {
				const char* line_end = (line_no < line_offsets_.Size) ? buf_begin + line_offsets_[line_no] : NULL;
				if (filter_.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
		} else {
			ImGui::TextUnformatted(buf_.begin());
		}

		if (scroll_to_bottom_)
			ImGui::SetScrollHere(1.0f);
		scroll_to_bottom_ = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::End();
	}

} // namespace sakura::editor::widgets