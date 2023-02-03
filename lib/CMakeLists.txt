cmake_minimum_required(VERSION 3.16)
project(DecompilerLib)

set(CMAKE_CXX_STANDARD 20)


add_library(DecompilerLib
        source/disasm/disassembler.cpp
        source/ast/ast_node.cpp
        )

target_include_directories(DecompilerLib PUBLIC include)
target_link_libraries(DecompilerLib PUBLIC fmt::fmt)