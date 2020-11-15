cmake_minimum_required(VERSION 3.6)

add_subdirectory(test_plugin)
set_target_properties (test_plugin PROPERTIES FOLDER plugins/game)