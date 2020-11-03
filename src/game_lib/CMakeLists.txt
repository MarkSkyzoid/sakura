project(sakura_game_lib)

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

file(GLOB_RECURSE sakura_game_lib_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"*.hpp"
	"*.cpp"
    "private/*.hpp"
    "private/*.cpp"
)

include_directories(${sakura_game_lib_SOURCE_DIR})
include_directories(${sakura_SOURCE_DIR})

add_library(sakura_game_lib
	${sakura_game_lib_SRC}
)

assign_source_group(${sakura_game_lib_SRC})

# Link and create SDL2 Definition if we're using SDL2
if (SDL2_ENABLED)
	find_package(SDL2 REQUIRED)
	if (SDL2_FOUND)
        # Concatenate SDL2_Dir and relative path(suffix of either x64 or x86) here.
		include_directories(${SDL2_INCLUDE_DIRS})
	endif()
endif()

target_link_libraries(sakura_game_lib sakura)