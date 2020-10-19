# Macros
macro(add_post_build_steps libraryPath)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
     add_custom_command(TARGET sakura_examples POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${libraryPath}/SDL2.dll
         $<TARGET_FILE_DIR:sakura_examples>/SDL2.dll)
    endif()
endmacro()

project(sakura_examples)

include_directories(${sakura_SOURCE_DIR})

file(GLOB sakura_examples_SRC
	"*.hpp"
	"*.cpp"
)

add_executable(sakura_examples
	${sakura_examples_SRC}
)

# Link and create SDL2 Definition if we're using SDL2
if (SDL2_ENABLED)
	find_package(SDL2 REQUIRED)
	if (SDL2_FOUND)
        # Concatenate SDL2_Dir and relative path(suffix of either x64 or x86) here.
		get_filename_component(SDL2_LIB_DIR ${SDL2_DIR}/${VC_LIB_PATH_SUFFIX} ABSOLUTE)
		add_post_build_steps(${SDL2_LIB_DIR})
	endif()
endif()

target_link_libraries(sakura_examples sakura)