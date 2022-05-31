cmake_minimum_required(VERSION 3.16)
project(Decompiler)

set(CMAKE_CXX_STANDARD 20)

if (NOT TARGET fmt::fmt)
    add_subdirectory(external/fmt EXCLUDE_FROM_ALL)
    set_property(TARGET fmt PROPERTY POSITION_INDEPENDENT_CODE ON)
endif ()

add_subdirectory(lib)
add_subdirectory(test)
