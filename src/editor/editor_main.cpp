#include "app.hpp"
#include "log/log.hpp"
#include "SDL.h"

#include "../game_lib/game.hpp"

#include "imgui.h"
#include "../ext/imgui_sdl/imgui_sdl.h"
#include "backends/imgui_impl_sdl.h"

#include "widgets/widgets.hpp"
#include "imgui_internal.h"

#include "scene/scene.hpp"

constexpr sakura::i32 WIDTH = 1024;
constexpr sakura::i32 HEIGHT = 768;

SDL_Renderer* g_renderer = nullptr;
SDL_Texture* g_scene_texture = nullptr;

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
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge = true;
	// io.ConfigViewportsNoTaskBarIcon = true;
	// Setup backend capabilities flags
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)

	io.BackendPlatformName = "imgui_impl_sdl";

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

	io.ClipboardUserData = NULL;

	// Setup Dear ImGui style
	{
		ImFontConfig config;
		ImFont* lato = io.Fonts->AddFontFromFileTTF("assets/fonts/Lato/Lato-Regular.ttf", 16.0f, &config);
		if (lato) {
			io.FontDefault = lato;
		}
	}
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGuiSDL::Initialize(g_renderer, WIDTH, HEIGHT);

	// Init game
	sakura::game_lib::init(app, g_editor_scene.ecs);
}
void cleanup(const sakura::App& app)
{
	// Clean up game first
	sakura::game_lib::cleanup(app, g_editor_scene.ecs);

	ImGuiSDL::Deinitialize();
	SDL_DestroyRenderer(g_renderer);
	ImGui::DestroyContext();
}
void fixed_update(sakura::f32 dt, const sakura::App& app)
{
	if (g_play_state == PlayState::Playing) {
		sakura::game_lib::fixed_update(dt, app, g_editor_scene.ecs);
	} else {
		sakura::game_lib::fixed_update(0.0f, app, g_editor_scene.ecs);
	}
}

void ImGui_UpdateMousePosAndButtons()
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseHoveredViewport = 0;

	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

	// [2]
	// Set Dear ImGui mouse pos from OS mouse pos + get buttons. (this is the common behavior)
	int mouse_x_local, mouse_y_local;
	Uint32 mouse_buttons = SDL_GetMouseState(&mouse_x_local, &mouse_y_local);

	// SDL 2.0.3 and before: single-viewport only
	io.MousePos = ImVec2((float)mouse_x_local, (float)mouse_y_local);
}

float menu_bar_height = 0;
ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

void DockSpaceUI(float toolbar_size)
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

void MenuBarUI()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
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

void update(sakura::f32 dt, const sakura::App& app)
{
	auto editor_dt = g_editor_clock.delta_time_seconds();
	sakura::game_lib::update(editor_dt, app, g_editor_scene.ecs);
	g_editor_clock.update(dt);

	ImGui_ImplSDL2_NewFrame(static_cast<SDL_Window*>(app.native_window_handle()));
	ImGui_UpdateMousePosAndButtons();

	ImGui::NewFrame();

	// ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	DockSpaceUI(g_widgets.toolbar.size);
	MenuBarUI();
	g_widgets.toolbar.draw(ImGui::GetMainViewport(), menu_bar_height);

	g_widgets.scene_browser.draw(g_editor_scene);
	bool b_asset_viewer_open = true;
	ImGui::Begin("Assets", &b_asset_viewer_open);
	ImGui::End();

	bool b_inspector_open = true;
	ImGui::Begin("Inspector", &b_inspector_open);
	ImGui::End();

	bool b_game_scene_open = true;
	char game_window_title_buf[256];
	sprintf_s(game_window_title_buf, "Game - FPS: %f###Game", 1.0f / dt);
	ImGui::Begin(game_window_title_buf, &b_game_scene_open);
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;
		ImGui::Image(g_scene_texture, ImVec2(vMax.x - vMin.x, vMax.y - vMin.y));
	}
	ImGui::End();

	bool log_window_opened = true;
	g_widgets.log_window.Draw("Log", &log_window_opened);

	bool b_demo_window_open = false;
	ImGui::ShowDemoWindow(&b_demo_window_open);
}
void render(sakura::f32 dt, sakura::f32 frame_interpolator, const sakura::App& app)
{
	auto render_system = [&app](float delta_time, float interpolator, SDL_Renderer* renderer) {
		// Render game scene
		SDL_SetRenderTarget(renderer, g_scene_texture);
		sakura::game_lib::render(delta_time, interpolator, app, g_editor_scene.ecs, renderer);

		// Render imgui
		SDL_SetRenderTarget(renderer, NULL);
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		SDL_RenderPresent(renderer);
	};

	auto editor_dt = g_editor_clock.delta_time_seconds();
	render_system(editor_dt, frame_interpolator, g_renderer);
}

void native_message_pump(void* data)
{
	SDL_Event e = *static_cast<SDL_Event*>(data);
	ImGui_ImplSDL2_ProcessEvent(&e);
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
							.set_native_message_pump_callback(native_message_pump);

	app.run();

	return 0;
}