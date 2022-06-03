cmake_minimum_required(VERSION 3.15)
project(RegexFiniteAutomata)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "Debug")


aux_source_directory(. DIR_SRCS)

add_subdirectory(FiniteAutomata)
add_subdirectory(utils)

add_executable(RegexFiniteAutomata main.cpp utils/utils.cpp)
add_executable(FAGraphGenerator FAGraphGenerator.cpp utils/utils.cpp)

target_link_libraries(RegexFiniteAutomata FiniteAutomata)
target_link_libraries(FAGraphGenerator FiniteAutomata)