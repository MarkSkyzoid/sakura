cmake_minimum_required(VERSION 3.6)
project(sakura)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sakura
	
	src/test.cpp

)

# Platform Variables
set(SDL2_ENABLED true CACHE BOOL "Use SDL2 as the System implementation")
set(SDL2_INCLUDE_DIRS "" CACHE FILEPATH "SDL2 Headers")
set(SDL2_LIBRARIES "" CACHE FILEPATH "SDL2 Lib")
set(SDL2_MAIN_LIBRARIES "" CACHE FILEPATH "SDL2 Main Lib")

# Link and create SDL2 Definition if we're using SDL2
if (SDL2_ENABLED)
	add_compile_definitions(SAKURA_USE_SDL2)
	target_include_directories(sakura PUBLIC "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
	target_link_libraries(sakura PUBLIC ${SDL2_LIBRARIES})
	target_link_libraries(sakura PUBLIC ${SDL2_MAIN_LIBRARIES})
endif()