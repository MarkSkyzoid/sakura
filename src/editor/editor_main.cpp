#include "app.hpp"
#include "log/log.hpp"
#include "imgui/imgui.hpp"
#include "SDL.h"

#include "../game_lib/game.hpp"

#define IM_VEC2_CLASS_EXTRA                                               \
	ImVec2 operator*(const float& r) { return ImVec2(x * r, y * r); }      \
	ImVec2 operator+(const ImVec2& r) { return ImVec2(x + r.x, y + r.y); } \
	ImVec2 operator-(const ImVec2& r) { return ImVec2(x - r.x, y - r.y); }

#include "../ext/imgui/imgui.h"
#include "../ext/imgui_sdl/imgui_sdl.h"

#include "../ext/IconFontCppHeaders/IconsFontAwesome5.h"

#include "widgets/widgets.hpp"
#include "../ext/imgui/imgui_internal.h"

#include "components/components.hpp"
#include "scene/scene.hpp"

constexpr sakura::i32 WIDTH = 1024;
constexpr sakura::i32 HEIGHT = 768;

static SDL_Renderer* g_renderer = nullptr;
static SDL_Texture* g_scene_texture = nullptr;
static ImFont* g_defaultFont = nullptr;
static ImFont* g_defaultBoldFont = nullptr;

static sakura::editor::Scene g_editor_scene;
static sakura::editor::Widgets g_widgets;

enum class PlayState
{
	Stopped = 0,
	Paused,
	Playing,
	Count
};
PlayState g_play_state = PlayState::Playing;
sakura::Clock g_editor_clock;
sakura::ImGuiLayer g_imgui_layer;

static float g_FPS = 0.0f;

inline void SetupImGuiStyle(bool bStyleDark_, float alpha_)
{
	constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b) {
		return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
	};

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
	const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
	const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

	const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
	const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
	const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

	const ImVec4 textColor = ColorFromBytes(255, 255, 255);
	const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
	const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

	colors[ImGuiCol_Text] = textColor;
	colors[ImGuiCol_TextDisabled] = textDisabledColor;
	colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_PopupBg] = bgColor;
	colors[ImGuiCol_Border] = borderColor;
	colors[ImGuiCol_BorderShadow] = borderColor;
	colors[ImGuiCol_FrameBg] = panelColor;
	colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
	colors[ImGuiCol_FrameBgActive] = panelActiveColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	colors[ImGuiCol_TitleBgActive] = bgColor;
	colors[ImGuiCol_TitleBgCollapsed] = bgColor;
	colors[ImGuiCol_MenuBarBg] = panelColor;
	colors[ImGuiCol_ScrollbarBg] = panelColor;
	colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
	colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
	colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
	colors[ImGuiCol_CheckMark] = panelActiveColor;
	colors[ImGuiCol_SliderGrab] = panelHoverColor;
	colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
	colors[ImGuiCol_Button] = panelColor;
	colors[ImGuiCol_ButtonHovered] = panelHoverColor;
	colors[ImGuiCol_ButtonActive] = panelHoverColor;
	colors[ImGuiCol_Header] = panelColor;
	colors[ImGuiCol_HeaderHovered] = panelHoverColor;
	colors[ImGuiCol_HeaderActive] = panelActiveColor;
	colors[ImGuiCol_Separator] = borderColor;
	colors[ImGuiCol_SeparatorHovered] = borderColor;
	colors[ImGuiCol_SeparatorActive] = borderColor;
	colors[ImGuiCol_ResizeGrip] = bgColor;
	colors[ImGuiCol_ResizeGripHovered] = panelColor;
	colors[ImGuiCol_ResizeGripActive] = lightBgColor;
	colors[ImGuiCol_PlotLines] = panelActiveColor;
	colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
	colors[ImGuiCol_PlotHistogram] = panelActiveColor;
	colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
	colors[ImGuiCol_ModalWindowDarkening] = bgColor;
	colors[ImGuiCol_DragDropTarget] = bgColor;
	colors[ImGuiCol_NavHighlight] = bgColor;
	colors[ImGuiCol_DockingPreview] = panelActiveColor;
	colors[ImGuiCol_Tab] = bgColor;
	colors[ImGuiCol_TabActive] = panelActiveColor;
	colors[ImGuiCol_TabUnfocused] = bgColor;
	colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
	colors[ImGuiCol_TabHovered] = panelHoverColor;

	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.TabRounding = 0.0f;

	if (!bStyleDark_) {
		for (int i = 0; i <= ImGuiCol_COUNT; i++) {
			ImVec4& col = style.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f) {
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
			if (col.w < 1.00f) {
				col.w *= alpha_;
			}
		}
	} else {
		for (int i = 0; i <= ImGuiCol_COUNT; i++) {
			ImVec4& col = style.Colors[i];
			if (col.w < 1.00f) {
				col.x *= alpha_;
				col.y *= alpha_;
				col.z *= alpha_;
				col.w *= alpha_;
			}
		}
	}
}

void init(const sakura::App& app)
{
	// #SK_TODO: Delete when we have a rendering backend!
	g_renderer =
	SDL_CreateRenderer((SDL_Window*)sakura::platform::get_native_window_handle(app.platform_handle()),
							 -1, SDL_RENDERER_SOFTWARE);

	g_scene_texture =
	SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

	// Widgets
	sakura::logging::add_callback("editor_log_window", sakura::editor::Widgets::log_callback,
											&g_widgets.log_window, sakura::logging::NamedVerbosity::Verbosity_MAX);

	g_editor_clock.start_with_time_now();

	g_widgets.toolbar.play_press_callback = []() {
		g_play_state = PlayState::Playing;
		g_editor_clock.set_is_paused(false);
	};
	g_widgets.toolbar.pause_press_callback = []() {
		g_play_state = PlayState::Paused;
		g_editor_clock.set_is_paused(true);
	};
	g_widgets.toolbar.stop_press_callback = []() {
		g_play_state = PlayState::Stopped;
		g_editor_clock.set_is_paused(true);
	};

	// Imgui
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	// Config
	io.IniFilename = "assets/config/editor/imgui.ini";

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	{
		SetupImGuiStyle(true, 1.0f);
		ImGuiIO& io = ImGui::GetIO();
		// io.Fonts->AddFontDefault();
		ImFontConfig default_font_config;
		default_font_config.OversampleH = default_font_config.OversampleV = 1;
		default_font_config.PixelSnapH = true;
		io.FontDefault = g_defaultFont =
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Medium.ttf", 15.0f, &default_font_config);

		// merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF("assets/fonts/FontAwesome5/" FONT_ICON_FILE_NAME_FAS, 15.0f,
											  &icons_config, icons_ranges);
		// use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid

		g_defaultBoldFont =
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Black.ttf", 15.0f, &default_font_config);
		io.Fonts->AddFontFromFileTTF("assets/fonts/FontAwesome5/" FONT_ICON_FILE_NAME_FAS, 15.0f,
											  &icons_config, icons_ranges);
	}
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	g_imgui_layer.init(static_cast<SDL_Window*>(app.native_window_handle()), WIDTH, HEIGHT);
	ImGuiSDL::Initialize(g_renderer, WIDTH, HEIGHT);

	// Init game
	sakura::game_lib::init(app, g_editor_scene.ecs);
}
void cleanup(const sakura::App& app)
{
	// Clean up game first
	sakura::game_lib::cleanup(app, g_editor_scene.ecs);

	g_imgui_layer.cleanup();
	SDL_DestroyRenderer(g_renderer);
}

static float menu_bar_height = 0;
void draw_dockspace_ui(float toolbar_size)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, toolbar_size));
	ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, toolbar_size));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
											  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
											  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
											  ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("Master DockSpace", NULL, window_flags);
	ImGuiID dock_main = ImGui::GetID("MyDockspace");

	// Save off menu bar height for later.
	menu_bar_height = ImGui::GetCurrentWindow()->MenuBarHeight();

	ImGui::DockSpace(dock_main);
	ImGui::End();
	ImGui::PopStyleVar(3);
}

void draw_menubar_ui()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			static bool b_dark_theme = true;
			if (ImGui::Checkbox("Dark theme", &b_dark_theme)) {
				SetupImGuiStyle(b_dark_theme, 1.0f);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
			} // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {
			}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {
			}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

struct ImguiFrameBlocker
{
	bool did_update = false;
} g_imgui_frameblocker;

void imgui_update(sakura::f32 dt, const sakura::App& app)
{
	g_imgui_layer.new_frame(static_cast<SDL_Window*>(app.native_window_handle()));

	draw_dockspace_ui(g_widgets.toolbar.size);
	draw_menubar_ui();
	g_widgets.toolbar.draw(ImGui::GetMainViewport(), menu_bar_height);

	g_widgets.scene_browser.draw(ICON_FA_GLOBE_EUROPE " Scene###Scene", g_editor_scene);
	g_widgets.entity_inspector.draw(
	ICON_FA_EYE " Inspector###Inspector", g_editor_scene, g_widgets.scene_browser.get_selected_entity(),
	[](sakura::ser::ImGuiEntityInspectorWalker& visitor, sakura::ecs::ECS& ecs, sakura::ecs::Entity entity) {
		sakura::components::visit_components<sakura::ser::ImGuiEntityInspectorWalker>(visitor, ecs, entity);
		sakura::game_lib::visit_components<sakura::ser::ImGuiEntityInspectorWalker>(visitor, ecs, entity);
	});
	static bool b_asset_viewer_open = true;
	ImGui::Begin(ICON_FA_FOLDER_OPEN " Assets###Assets", &b_asset_viewer_open);
	{
#define DO_FOLDER(FolderName, Color, Icon)                                    \
	static bool b_##FolderName = false;                                        \
	const char* FolderName##_label = (Icon " " #FolderName "###" #FolderName); \
	ImGui::PushStyleColor(ImGuiCol_Text, Color.Value);                         \
	ImGui::PushFont(g_defaultBoldFont);                                        \
	if (ImGui::TreeNode(FolderName##_label)) {                                 \
		ImGui::PopFont();                                                       \
		b_##FolderName = true;                                                  \
		ImGui::Bullet();                                                        \
		ImGui::Selectable(#FolderName "1");                                     \
		ImGui::Bullet();                                                        \
		ImGui::Selectable(#FolderName "2");                                     \
		ImGui::Bullet();                                                        \
		ImGui::Selectable(#FolderName "3");                                     \
		ImGui::TreePop();                                                       \
	} else {                                                                   \
		ImGui::PopFont();                                                       \
		b_##FolderName = false;                                                 \
	}                                                                          \
	ImGui::PopStyleColor();

		ImGui::TreePush("FilesSystem");
		{
			ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
			DO_FOLDER(Sprites, ImColor(251, 241, 98), ICON_FA_LEMON);
			DO_FOLDER(Audio, ImColor(240, 96, 103), ICON_FA_RECORD_VINYL);
			DO_FOLDER(Scripts, ImColor(88, 175, 252), ICON_FA_BUG);
		}
		ImGui::TreePop();
	}
	ImGui::End();

	bool b_game_scene_open = true;
	char game_window_title_buf[256];
	sprintf_s(game_window_title_buf, "Game - FPS: %f###Game", g_FPS);
	ImGui::Begin(game_window_title_buf, &b_game_scene_open);
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		const auto content_width = vMax.x - vMin.x;
		const auto content_height = vMax.y - vMin.y;
		const auto scale = std::min(content_width / WIDTH, content_height / HEIGHT);
		const auto image_size = ImVec2(WIDTH * scale, HEIGHT * scale);
		ImGui::SetCursorPos((ImGui::GetWindowSize() - image_size) * 0.5f);
		ImGui::Image(g_scene_texture, image_size);
	}
	ImGui::End();

	g_widgets.log_window.draw(ICON_FA_SCROLL " Output###Log");

	// bool b_demo_window_open = false;
	// ImGui::ShowDemoWindow(&b_demo_window_open);

	g_imgui_frameblocker.did_update = true;
}

void fixed_update(sakura::f32 dt, const sakura::App& app)
{
	if (g_play_state == PlayState::Playing) {
		sakura::game_lib::fixed_update(dt, app, g_editor_scene.ecs);
	} else {
		sakura::game_lib::fixed_update(0.0f, app, g_editor_scene.ecs);
	}

	if (!g_imgui_frameblocker.did_update) {
		imgui_update(dt, app);
	}
}

void update(sakura::f32 dt, const sakura::App& app)
{
	auto editor_dt = g_editor_clock.delta_time_seconds();
	sakura::game_lib::update(editor_dt, app, g_editor_scene.ecs);
	g_FPS = 1.0f / editor_dt;
}

void render(sakura::f32 dt, sakura::f32 frame_interpolator, const sakura::App& app)
{
	auto render_system = [&app](float delta_time, float interpolator, SDL_Renderer* renderer) {
		// Render game scene
		SDL_SetRenderTarget(renderer, g_scene_texture);
		sakura::game_lib::render(delta_time, interpolator, app, g_editor_scene.ecs, renderer);

		// Render imgui
		SDL_SetRenderTarget(renderer, NULL);

		if (g_imgui_frameblocker.did_update) {
			g_imgui_layer.render();
			ImGuiSDL::Render(ImGui::GetDrawData());

			g_imgui_frameblocker.did_update = false;
		}

		SDL_RenderPresent(renderer);
	};

	auto editor_dt = g_editor_clock.delta_time_seconds();
	render_system(editor_dt, frame_interpolator, g_renderer);
}

void end_of_main_loop_update(sakura::f32 dt, const sakura::App& app)
{
	g_editor_clock.update(dt);
}

void native_message_pump(void* data)
{
	SDL_Event* e = static_cast<SDL_Event*>(data);
	g_imgui_layer.on_event(e);
}

int main(int argc, char* argv[])
{

	sakura::App app = sakura::App::Builder()
							.set_title("Sakura Editor")
							.set_width(WIDTH)
							.set_height(HEIGHT)
							.set_resizable(true)
							.set_target_frame_rate(60.0f)
							.set_init_callback(init)
							.set_cleanup_callback(cleanup)
							.set_fixed_update_callback(fixed_update)
							.set_update_callback(update)
							.set_render_callback(render)
							.set_end_of_main_loop_update_callback(end_of_main_loop_update)
							.set_native_message_pump_callback(native_message_pump);

	app.run();

	return 0;
}