cmake_minimum_required(VERSION 3.10)
project(arena C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

include_directories(deps)
include(deps/option/option.cmake)
include(deps/panic/panic.cmake)

add_library(arena sources/arena_config.h sources/arena.h sources/arena.c)
target_link_libraries(arena PUBLIC option)

add_executable(main examples/main.c)
target_link_libraries(main PRIVATE arena)
target_include_directories(main PRIVATE sources)
