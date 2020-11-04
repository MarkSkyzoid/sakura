# Project setup
cmake_minimum_required(VERSION 3.6)
project(sakura)

# CMake config
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sakura_editor)

add_subdirectory(src/sakura)
add_subdirectory(src/editor)
add_subdirectory(src/launcher)
add_subdirectory(src/game_lib)

set_property(TARGET sakura_editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")