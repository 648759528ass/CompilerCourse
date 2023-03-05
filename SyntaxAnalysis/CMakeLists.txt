cmake_minimum_required(VERSION 3.15)
project(SyntaxAnalysis)
#add_compile_options(-Wall)
#add_compile_options(-O3)
#add_compile_options(-GF)
set(CMAKE_CXX_STANDARD 20)


add_library(Grammar Grammar/Grammar.cpp)
add_library(errorHandle ErrorHandle/errorHandle.cpp)


add_subdirectory(Grammar)
add_subdirectory(LL1)
add_subdirectory(LRSeries)

add_executable(SyntaxAnalysis main.cpp)
target_link_libraries(SyntaxAnalysis Grammar errorHandle LL1 LR)