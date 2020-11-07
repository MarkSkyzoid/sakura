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
#include "../../../../ext/imgui/backends/imgui_impl_sdl.h"
#include "log/log.hpp"

#include "SDL.h"
#include "SDL_syswm.h"

#include <algorithm>

namespace sakura {
	void ImGuiLayer::init(void* native_window_handle, float width, float height)
	{
		width_ = width;
		height_ = height;
		ImGuiIO& io = ImGui::GetIO();

		// Setup backend capabilities flags
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

		ImGui_ImplSDL2_InitForOpenGL(static_cast<SDL_Window*>(native_window_handle), nullptr);
	}

	void ImGuiLayer::cleanup()
	{
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::new_frame(void* native_window_handle)
	{
		ImGui_ImplSDL2_NewFrame(static_cast<SDL_Window*>(native_window_handle));
		ImGui::NewFrame();
	}

	void ImGuiLayer::render()
	{
		if (visible()) {
			ImGui::Render();
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
	}

	void ImGuiLayer::on_event(const void* event_data_ptr)
	{
		if (!interactive() || !event_data_ptr) {
			return;
		}

		const SDL_Event& event = *static_cast<const SDL_Event*>(event_data_ptr);
		ImGui_ImplSDL2_ProcessEvent(&event);
	}
} // namespace sakura

#endif // SAKURA_USE_SDL2
