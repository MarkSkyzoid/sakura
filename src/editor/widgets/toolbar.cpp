#include "toolbar.hpp"
#include "imgui.h"

#include "../../ext/IconFontCppHeaders/IconsFontAwesome5.h"

void sakura::editor::widgets::Toolbar::draw(ImGuiViewport* viewport, float menu_bar_height)
{
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menu_bar_height));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, size));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
											  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
											  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("TOOLBAR", NULL, window_flags);
	ImGui::PopStyleVar();

	add_buttons();

	size = ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y;

	ImGui::End();
}

void sakura::editor::widgets::Toolbar::add_buttons()
{
	const float item_spacing = ImGui::GetStyle().ItemSpacing.x;

	static float play_button_width = 10.0f;  // Estimate, only for first frame
	static float pause_button_width = 10.0f; // Estimate, only for first frame
	static float stop_button_width = 10.0f;  // Estimate, only for first frame

	const float total_width = (play_button_width + pause_button_width + stop_button_width) + 3.0f * item_spacing;
	const float start_pos = ImGui::GetWindowWidth() * 0.5f - total_width * 0.5f;

#define DO_BUTTON(name, icon)                                                                               \
	{                                                                                                        \
		const auto font_size = ImGui::GetFontSize();                                                          \
		const float font_size_padded = ImGui::GetFrameHeight();                                               \
		const auto padding = (font_size_padded - font_size);                                                  \
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, padding));                                  \
		if (ImGui::Button(icon, ImVec2(font_size_padded + 2, font_size_padded + 2)) && name##_press_callback) \
			name##_press_callback();                                                                           \
		ImGui::PopStyleVar(1);                                                                                \
	}

	float pos = start_pos;
	ImGui::SameLine(pos);
	DO_BUTTON(play, ICON_FA_PLAY);
	play_button_width = ImGui::GetItemRectSize().x; // Get the actual width for next frame.

	pos += play_button_width + item_spacing;
	ImGui::SameLine(pos);
	DO_BUTTON(pause, ICON_FA_PAUSE);
	pause_button_width = ImGui::GetItemRectSize().x; // Get the actual width for next frame.

	pos += pause_button_width + item_spacing;
	ImGui::SameLine(pos);
	DO_BUTTON(stop, ICON_FA_STOP);
	stop_button_width = ImGui::GetItemRectSize().x; // Get the actual width for next frame.

	pos += stop_button_width + item_spacing;

#undef DO_BUTTON
}
