cmake_minimum_required(VERSION 3.25)
project(LFA_Tema_2)
set(CMAKE_CXX_STANDARD 20)
add_library(
        FA STATIC
        FA.h
        FA.cpp
        DFA.h
        DFA.cpp
        NFA.h
        NFA.cpp
)
add_executable(LFA_Tema_2 main.cpp)
add_executable(Random_Test_Generator random_test_generator.cpp)
target_link_libraries(LFA_Tema_2 PRIVATE FA)
target_link_libraries(Random_Test_Generator PRIVATE FA)
