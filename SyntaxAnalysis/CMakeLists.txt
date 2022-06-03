cmake_minimum_required(VERSION 3.15)
project(SyntaxAnalysis)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(Libsymbol)

add_subdirectory(Libgrammar)

add_executable(SyntaxAnalysis main.cpp)