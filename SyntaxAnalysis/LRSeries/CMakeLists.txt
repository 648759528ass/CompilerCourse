add_executable(LRTest LR0.cpp LRTest.cpp SLR.cpp LRAnalyse.cpp LR1.cpp LALR.cpp)
target_link_libraries(LRTest Grammar errorHandle)
add_library(LR LR0.cpp LRTest.cpp SLR.cpp LRAnalyse.cpp LR1.cpp LALR.cpp)