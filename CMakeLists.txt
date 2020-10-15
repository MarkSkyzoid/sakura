# Macros
macro(add_post_build_steps libraryPath)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
     add_custom_command(TARGET sakura POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         ${libraryPath}/SDL2.dll
         $<TARGET_FILE_DIR:sakura>/SDL2.dll)
    endif()
endmacro()

# Project setup
cmake_minimum_required(VERSION 3.6)
project(sakura)

# CMake config
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB sakura_SRC
	"src/*.hpp"
	"src/*.cpp"
)

add_executable(sakura
	${sakura_SRC}
)

# Properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sakura)

# Platform Variables
set(SDL2_ENABLED true CACHE BOOL "Use SDL2 as the System implementation")

# Link and create SDL2 Definition if we're using SDL2
if (SDL2_ENABLED)
	find_package(SDL2 REQUIRED)
	if (SDL2_FOUND)
		# Concatenate SDL2_Dir and relative path(suffix of either x64 or x86) here.
		get_filename_component(SDL2_LIB_DIR ${SDL2_DIR}/${VC_LIB_PATH_SUFFIX} ABSOLUTE)
		include_directories(${SDL2_INCLUDE_DIRS})
		add_post_build_steps(${SDL2_LIB_DIR})
		target_link_libraries(sakura ${SDL2_LIBRARIES})
	endif()
endif()