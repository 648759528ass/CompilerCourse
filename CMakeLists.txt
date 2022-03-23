cmake_minimum_required(VERSION 3.17)
project(CompilerCourse)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "Debug")


aux_source_directory(. DIR_SRCS)

add_subdirectory(FiniteAutomata)
add_subdirectory(utils)

add_executable(CompilerCourse main.cpp)
add_executable(FAGraphGenerator FAGraphGenerator.cpp)

target_link_libraries(CompilerCourse FiniteAutomata utils)
target_link_libraries(FAGraphGenerator FiniteAutomata utils)