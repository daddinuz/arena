cmake_minimum_required(VERSION 3.8)
project(arena C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

# dependencies
include_directories(deps)
include(deps/panic/build.cmake)
include(deps/option/build.cmake)
include(deps/alligator/build.cmake)

# archive
include_directories(sources)
include(sources/build.cmake)

# exmaples
include(examples/build.cmake)
