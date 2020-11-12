project(test_plugin)

macro(add_post_build_steps libraryPath)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
     add_custom_command(TARGET test_plugin POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${libraryPath}/SDL2.dll
         $<TARGET_FILE_DIR:test_plugin>/SDL2.dll)
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

file(GLOB_RECURSE test_plugin_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"*.hpp"
	"*.cpp"
    "private/*.hpp"
    "private/*.cpp"
)

include_directories(${test_plugin_SOURCE_DIR})
include_directories(${sakura_SOURCE_DIR})

if(PLUGIN_HOTRELOAD)
add_definitions(-DTEST_PLUGIN_EXPORTS)

add_library(test_plugin SHARED
	${test_plugin_SRC}
)
else()
add_library(test_plugin
	${test_plugin_SRC}
)
endif()

set_target_properties(test_plugin
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SAKURA_BIN_DIR}/../plugins"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SAKURA_BIN_DIR}/../plugins"
)

assign_source_group(${sakura_game_lib_SRC})

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


target_link_libraries(test_plugin sakura)
