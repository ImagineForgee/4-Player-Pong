project(server)

file(GLOB_RECURSE SOURCES "**.cpp" "**.hpp")
add_executable(server ${SOURCES})
target_link_libraries(server common purrsock)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/common)