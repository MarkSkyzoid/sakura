#ifdef SAKURA_USE_SDL2

#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE SDL_VERSION_ATLEAST(2, 0, 4)
#define SDL_HAS_WINDOW_ALPHA SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_ALWAYS_ON_TOP SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_USABLE_DISPLAY_BOUNDS SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_PER_MONITOR_DPI SDL_VERSION_ATLEAST(2, 0, 4)
#define SDL_HAS_VULKAN SDL_VERSION_ATLEAST(2, 0, 6)
#define SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH SDL_VERSION_ATLEAST(2, 0, 5)

#include "../../imgui.hpp"
#include "../../../../ext/imgui/imgui.h"
#include "log/log.hpp"

#include "SDL.h"

#include <algorithm>

namespace sakura {
	static const char* get_clipboard_text_fn(void*) { return SDL_GetClipboardText(); }

	static void set_clipboard_text_fn(void*, const char* text) { SDL_SetClipboardText(text); }

	void ImGuiLayer::init(float width, float height)
	{
		width_ = width;
		height_ = height;

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = { width_, height_ };
		io.DeltaTime = 1.0f / 60.0f; // Will be adjusted later, so just a placeholder here
		
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
		io.BackendFlags |=
		ImGuiBackendFlags_PlatformHasViewports; // We can create multi-viewports on the Platform side (optional)
#endif
		// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
		io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
		io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
		io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
		io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
		io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
		io.KeyMap[ImGuiKey_A] = SDLK_a;
		io.KeyMap[ImGuiKey_C] = SDLK_c;
		io.KeyMap[ImGuiKey_V] = SDLK_v;
		io.KeyMap[ImGuiKey_X] = SDLK_x;
		io.KeyMap[ImGuiKey_Y] = SDLK_y;
		io.KeyMap[ImGuiKey_Z] = SDLK_z;

		io.SetClipboardTextFn = set_clipboard_text_fn;
		io.GetClipboardTextFn = get_clipboard_text_fn;
	}

	void ImGuiLayer::cleanup() { ImGui::DestroyContext(); }

	void ImGuiLayer::new_frame()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Setup timestep:
		static double s_last_time = 0.0f;
		const double current_time = SDL_GetTicks() / 1000.0;
		io.DeltaTime = (float)(current_time - s_last_time);
		io.DeltaTime = std::max(io.DeltaTime, 0.0001f);
		s_last_time = current_time;

		if (interactive()) {
			// Setup inputs
			int mouse_x, mouse_y;
			auto mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
			io.MousePos = { (float)mouse_x, (float)mouse_y };
			io.MouseDown[0] = (mouse_state & SDL_BUTTON_LMASK);
			io.MouseDown[1] = (mouse_state & SDL_BUTTON_RMASK);
			io.MouseDown[2] = (mouse_state & SDL_BUTTON_MMASK);
		} else {
			io.MouseDown[0] = false;
			io.MouseDown[1] = false;
			io.MouseDown[2] = false;
			io.MousePos.x = -FLT_MAX;
			io.MousePos.y = -FLT_MAX;

			memset(io.KeysDown, 0, sizeof(io.KeysDown));
			io.KeyShift = false;
			io.KeyCtrl = false;
			io.KeyAlt = false;
			io.KeySuper = false;
		}

		// Hide OS mouse cursor if ImGui is drawing it
		SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

		// Start the frame
		ImGui::NewFrame();
	}

	void ImGuiLayer::render()
	{
		if (visible()) {
			ImGui::Render();
		}
	}

	void ImGuiLayer::on_event(const void* event_data_ptr)
	{
		if (!interactive() || !event_data_ptr) {
			return;
		}

		const SDL_Event& event = *static_cast<const SDL_Event*>(event_data_ptr);

		ImGuiIO& io = ImGui::GetIO();
		auto num_imgui_keys = sizeof(io.KeysDown) / sizeof(io.KeysDown[0]);

		switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			int key = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
			if (key < num_imgui_keys) {
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
			}
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
		} break;

		case SDL_MOUSEWHEEL: {
			io.MouseWheel = static_cast<float>(event.wheel.y);
		} break;

		case SDL_TEXTINPUT: {
			io.AddInputCharactersUTF8(event.text.text);
		} break;

		case SDL_WINDOWEVENT: {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				width_ = event.window.data1;
				height_ = event.window.data2;
				logging::dlog_info("Resized: %fx%f points", width_, height_);
				io.DisplaySize.x = width_;
				io.DisplaySize.y = height_;
			}
		} break;
		}
	}
} // namespace sakura

#endif // SAKURA_USE_SDL2
