#include "toolbar.hpp"
#include "imgui.h"

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

#define DO_BUTTON(name)                               \
	if (ImGui::Button(#name) && name##_press_callback) \
	name##_press_callback()

	float pos = start_pos;
	ImGui::SameLine(pos);
	DO_BUTTON(play);
	play_button_width = ImGui::GetItemRectSize().x; // Get the actual width for next frame.

	pos += play_button_width + item_spacing;
	ImGui::SameLine(pos);
	DO_BUTTON(pause);	
	pause_button_width = ImGui::GetItemRectSize().x; // Get the actual width for next frame.

	pos += pause_button_width + item_spacing;
	ImGui::SameLine(pos);
	DO_BUTTON(stop);
	stop_button_width = ImGui::GetItemRectSize().x; // Get the actual width for next frame.

	pos += stop_button_width + item_spacing;

#undef DO_BUTTON
}
