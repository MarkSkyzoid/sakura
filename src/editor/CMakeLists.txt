# Macros
macro(add_post_build_steps libraryPath)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
     add_custom_command(TARGET sakura_editor POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${libraryPath}/SDL2.dll
         $<TARGET_FILE_DIR:sakura_editor>/SDL2.dll)
    endif()
endmacro()

project(sakura_editor)

include_directories(${sakura_SOURCE_DIR})
include_directories(${sakura_SOURCE_DIR}/../ext/imgui/)

source_group(imgui FILES
	"${sakura_SOURCE_DIR}/../ext/imgui/imconfig.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/imgui_internal.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/imstb_rectpack.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/imstb_textedit.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/imstb_truetype.h"

	"${sakura_SOURCE_DIR}/../ext/imgui/imgui.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/imgui.cpp"
	"${sakura_SOURCE_DIR}/../ext/imgui/imgui_demo.cpp"
	"${sakura_SOURCE_DIR}/../ext/imgui/imgui_draw.cpp"
	"${sakura_SOURCE_DIR}/../ext/imgui/imgui_widgets.cpp"

	"${sakura_SOURCE_DIR}/../ext/imgui/backends/imgui_impl_sdl.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/backends/imgui_impl_sdl.cpp"
	
	
	"${sakura_SOURCE_DIR}/../ext/imgui_sdl/imgui_sdl.h"
	"${sakura_SOURCE_DIR}/../ext/imgui_sdl/imgui_sdl.cpp"
)

file(GLOB sakura_editor_SRC
	"*.hpp"
	"*.cpp"
	
	"${sakura_SOURCE_DIR}/../ext/imgui/*.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/*.cpp"

	"${sakura_SOURCE_DIR}/../ext/imgui/backends/imgui_impl_sdl.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/backends/imgui_impl_sdl.cpp"

	"${sakura_SOURCE_DIR}/../ext/imgui_sdl/imgui_sdl.h"
	"${sakura_SOURCE_DIR}/../ext/imgui_sdl/imgui_sdl.cpp"
)

add_executable(sakura_editor
	${sakura_editor_SRC}
)

# Link and create SDL2 Definition if we're using SDL2
if (SDL2_ENABLED)
	find_package(SDL2 REQUIRED)
	if (SDL2_FOUND)
        # Concatenate SDL2_Dir and relative path(suffix of either x64 or x86) here.
		include_directories(${SDL2_INCLUDE_DIRS})
		get_filename_component(SDL2_LIB_DIR ${SDL2_DIR}/${VC_LIB_PATH_SUFFIX} ABSOLUTE)
		add_post_build_steps(${SDL2_LIB_DIR})
	endif()
endif()

target_link_libraries(sakura_editor sakura)