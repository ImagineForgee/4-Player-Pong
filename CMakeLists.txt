cmake_minimum_required (VERSION 3.11)

project(raylibstarter)

add_subdirectory ("deps/purrsock")
include_directories ("deps/purrsock/include")

add_subdirectory ("game/client")
add_subdirectory ("game/server")