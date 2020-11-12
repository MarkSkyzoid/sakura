# Project setup
cmake_minimum_required(VERSION 3.6)
project(sakura)

# CMake config
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(SAKURA_BIN_DIR "${PROJECT_SOURCE_DIR}/bin")

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sakura_editor)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SAKURA_BIN_DIR}")

# Platform specific
if(WIN32)
	set(PLUGIN_HOTRELOAD true CACHE BOOL "Do plugin hot reloading")
else()
	set(PLUGIN_HOTRELOAD false CACHE BOOL "Do plugin hot reloading")
endif()

if(PLUGIN_HOTRELOAD)
add_definitions(-DSAKURA_PLUGIN_HOTRELOAD)
endif()

# Subdirs
add_subdirectory(src/sakura)
add_subdirectory(src/editor)
add_subdirectory(src/launcher)
add_subdirectory(src/game_lib)
add_subdirectory(src/test_plugin)

set_property(TARGET sakura_editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set_property(TARGET sakura_launcher PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")