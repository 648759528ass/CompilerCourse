cmake_minimum_required(VERSION 3.17)
project(CompilerCourse)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIR_SRCS)

add_subdirectory(FiniteAutomata)

add_executable(CompilerCourse ${DIR_SRCS})

target_link_libraries(CompilerCourse FiniteAutomata)