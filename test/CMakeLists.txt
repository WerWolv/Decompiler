cmake_minimum_required(VERSION 3.16)
project(DecompilerTest)

set(CMAKE_CXX_STANDARD 20)


add_executable(DecompilerTest
        source/main.cpp
)

target_link_libraries(DecompilerTest PUBLIC DecompilerLib)