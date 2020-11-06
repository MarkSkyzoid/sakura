# Macros
macro(add_post_build_steps libraryPath)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
     add_custom_command(TARGET sakura_editor POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${libraryPath}/SDL2.dll
         $<TARGET_FILE_DIR:sakura_editor>/SDL2.dll)
    endif()
endmacro()

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

project(sakura_editor)

file(GLOB_RECURSE sakura_editor_own_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"*.hpp"
	"*.cpp"
    "private/*.hpp"
    "private/*.cpp"
)

file(GLOB sakura_editor_SRC
   ${sakura_editor_own_SRC}
)

add_executable(sakura_editor
	${sakura_editor_SRC}
)

assign_source_group(${sakura_editor_SRC})

include_directories(${sakura_SOURCE_DIR})
include_directories(${sakura_SOURCE_DIR}/../ext/imgui/)
include_directories(${sakura_SOURCE_DIR}/../ext/IconFontCppHeaders/)

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
target_link_libraries(sakura_editor sakura_game_lib)