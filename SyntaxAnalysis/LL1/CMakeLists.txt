add_executable(LL1Test LL1Generation.cpp LL1Test.cpp)
target_link_libraries(LL1Test Grammar errorHandle)
add_library(LL1 LL1Generation.cpp LL1Test.cpp)