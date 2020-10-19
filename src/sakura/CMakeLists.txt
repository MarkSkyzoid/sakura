project(sakura)

file(GLOB sakura_SRC
	"*.hpp"
	"*.cpp"
)

add_library(sakura
	${sakura_SRC}
)

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