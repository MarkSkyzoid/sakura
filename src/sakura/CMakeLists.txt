project(sakura)

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

file(GLOB_RECURSE sakura_own_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"*.hpp"
	"*.cpp"
    "private/*.hpp"
    "private/*.cpp"
)
file(GLOB sakura_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	
	${sakura_own_SRC}

	"${sakura_SOURCE_DIR}/../ext/imgui/*.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/*.cpp"

	"${sakura_SOURCE_DIR}/../ext/imgui/backends/imgui_impl_sdl.h"
	"${sakura_SOURCE_DIR}/../ext/imgui/backends/imgui_impl_sdl.cpp"

	"${sakura_SOURCE_DIR}/../ext/imgui/misc/cpp/imgui_stdlib.cpp"

	"${sakura_SOURCE_DIR}/../ext/imgui_sdl/imgui_sdl.h"
	"${sakura_SOURCE_DIR}/../ext/imgui_sdl/imgui_sdl.cpp"
)

include_directories(${sakura_SOURCE_DIR})
include_directories(${sakura_SOURCE_DIR}/../ext/lecs)
include_directories(${sakura_SOURCE_DIR}/../ext/imgui/)
include_directories(${sakura_SOURCE_DIR}/../ext/IconFontCppHeaders/)

add_library(sakura
	${sakura_SRC}
)

assign_source_group(${sakura_SRC})

# Platform Variables
set(SDL2_ENABLED true CACHE BOOL "Use SDL2 as the System implementation")

# Link and create SDL2 Definition if we're using SDL2
if (SDL2_ENABLED)
	find_package(SDL2 REQUIRED)
	if (SDL2_FOUND)
		add_compile_definitions(SAKURA_USE_SDL2)
		include_directories(${SDL2_INCLUDE_DIRS})
		target_link_libraries(sakura ${SDL2_LIBRARIES})
	endif()
endif()