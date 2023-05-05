cmake_minimum_required(VERSION 3.25)
project(LFA_Tema_2)
set(CMAKE_CXX_STANDARD 20)
add_library(
        FA STATIC
        src/FA.h
        src/FA.cpp
        src/DFA.h
        src/DFA.cpp
        src/NFA.h
        src/NFA.cpp
)
add_executable(LFA_Tema_2 src/main.cpp)
add_executable(Random_Test_Generator src/random_test_generator.cpp)
target_link_libraries(LFA_Tema_2 PRIVATE FA)
target_link_libraries(Random_Test_Generator PRIVATE FA)
add_custom_target(copy_tests)
add_custom_command(TARGET copy_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/tests ${CMAKE_BINARY_DIR}/tests
        DEPENDS ${CMAKE_SOURCE_DIR}/tests
        COMMENT "Copying tests to build directory"
        VERBATIM
)
add_dependencies(LFA_Tema_2 copy_tests)
