unset(PANIC_UNWIND_SUPPORT CACHE)
unset(LIBUNWIND_FOUND CACHE)

set(ARCHIVE_NAME panic)
message("${ARCHIVE_NAME}@${CMAKE_CURRENT_LIST_DIR} using: ${CMAKE_CURRENT_LIST_FILE}")

file(GLOB ARCHIVE_HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB ARCHIVE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c)
add_library(${ARCHIVE_NAME} ${ARCHIVE_HEADERS} ${ARCHIVE_SOURCES})

# Optional features
option(PANIC_UNWIND_SUPPORT "Stack unwinding support" OFF)

###################
# Private section
####

# Find libunwind
# - Try to find Libunwind
#
# Input variables:
#  LIBUNWIND_ROOT_DIR     - The libunwind install directory
#  LIBUNWIND_INCLUDE_DIR  - The libunwind include directory
#  LIBUNWIND_LIBRARY      - The libunwind library directory
#
# Output variables:
#  LIBUNWIND_FOUND        - System has libunwind
#  LIBUNWIND_INCLUDE_DIRS - The libunwind include directories
#  LIBUNWIND_LIBRARIES    - The libraries needed to use libunwind
#  LIBUNWIND_VERSION      - The version string for libunwind

include(FindPackageHandleStandardArgs)

if (NOT DEFINED LIBUNWIND_FOUND)
    # Set default sarch paths for libunwind
    if (LIBUNWIND_ROOT_DIR)
        set(LIBUNWIND_INCLUDE_DIR ${LIBUNWIND_ROOT_DIR}/include CACHE PATH "The include directory for libunwind")
        if (CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
            set(LIBUNWIND_LIBRARY ${LIBUNWIND_ROOT_DIR}/lib64;${LIBUNWIND_ROOT_DIR}/lib CACHE PATH "The library directory for libunwind")
        else ()
            set(LIBUNWIND_LIBRARY ${LIBUNWIND_ROOT_DIR}/lib CACHE PATH "The library directory for libunwind")
        endif ()
    endif ()

    find_path(LIBUNWIND_INCLUDE_DIRS NAMES libunwind.h
            HINTS ${LIBUNWIND_INCLUDE_DIR})

    find_library(LIBUNWIND_LIBRARIES unwind
            HINTS ${LIBUNWIND_LIBRARY})

    # Get libunwind version
    if (EXISTS "${LIBUNWIND_INCLUDE_DIRS}/libunwind-common.h")
        # file(READ "${LIBUNWIND_INCLUDE_DIRS}/libunwind-common.h" _libunwind_version_header)
        string(REGEX REPLACE ".*define[ \t]+UNW_VERSION_MAJOR[ \t]+([0-9]+).*" "\\1"
                LIBUNWIND_MAJOR_VERSION "${_libunwind_version_header}")
        string(REGEX REPLACE ".*define[ \t]+UNW_VERSION_MINOR[ \t]+([0-9]+).*" "\\1"
                LIBUNWIND_MINOR_VERSION "${_libunwind_version_header}")
        string(REGEX REPLACE ".*define[ \t]+UNW_VERSION_EXTRA[ \t]+([0-9]+).*" "\\1"
                LIBUNWIND_MICRO_VERSION "${_libunwind_version_header}")
        set(LIBUNWIND_VERSION "${LIBUNWIND_MAJOR_VERSION}.${LIBUNWIND_MINOR_VERSION}.${LIBUNWIND_MICRO_VERSION}")
        unset(_libunwind_version_header)
    endif ()

    # handle the QUIETLY and REQUIRED arguments and set LIBUNWIND_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(libunwind
            FOUND_VAR LIBUNWIND_FOUND
            VERSION_VAR LIBUNWIND_VERSION
            REQUIRED_VARS LIBUNWIND_LIBRARIES LIBUNWIND_INCLUDE_DIRS)

    mark_as_advanced(LIBUNWIND_INCLUDE_DIR LIBUNWIND_LIBRARY
            LIBUNWIND_INCLUDE_DIRS LIBUNWIND_LIBRARIES)

endif ()

if (PANIC_UNWIND_SUPPORT)
    if (LIBUNWIND_FOUND)
        target_compile_definitions(${ARCHIVE_NAME} PUBLIC PANIC_UNWIND_SUPPORT=1)
        target_link_libraries(${ARCHIVE_NAME} PRIVATE unwind)
    else ()
        message(FATAL_ERROR "libunwind required for PANIC_UNWIND_SUPPORT feature but not found")
    endif (LIBUNWIND_FOUND)
else ()
    target_compile_definitions(${ARCHIVE_NAME} PUBLIC PANIC_UNWIND_SUPPORT=0)
endif (PANIC_UNWIND_SUPPORT)
