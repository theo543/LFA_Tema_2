cmake_minimum_required(VERSION 3.2)
project(LFA_Tema_2)
set(CMAKE_CXX_STANDARD 20)
option(SANITIZE "Enable sanitizers" OFF)
if(SANITIZE)
    message(STATUS "Sanitizers enabled")
    if(WIN32)
        message(WARNING "Sanitizers may fail to link on Windows")
    endif()
    set(CMAKE_CXX_FLAGS
        "-Wall -Wextra -Wno-sign-compare -pedantic -fsanitize=address,leak,undefined -fno-omit-frame-pointer -g"
    )
endif()
add_library(
        FA STATIC
        src/FA.h
        src/FA.cpp
        src/DFA.h
        src/DFA.cpp
        src/NFA.h
        src/NFA.cpp
        src/Acceptor.h
        src/utils.h src/utils.cpp
)
add_executable(LFA_Tema_2 src/main.cpp)
add_executable(Random_Test_Generator src/random_test_generator.cpp)
target_link_libraries(LFA_Tema_2 PRIVATE FA)
target_link_libraries(Random_Test_Generator PRIVATE FA)
add_custom_target(copy_tests)
add_custom_command(TARGET copy_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/tests
        COMMENT "Cleaning tests directory"
        VERBATIM
)
add_custom_command(TARGET copy_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/tests ${CMAKE_BINARY_DIR}/tests
        DEPENDS ${CMAKE_SOURCE_DIR}/tests
        COMMENT "Copying tests to build directory"
        VERBATIM
)
add_dependencies(LFA_Tema_2 copy_tests)
option(libfsm "Check results with libFSM" OFF)
if(libfsm)
    message(STATUS "libfsm enabled")
    include(FetchContent)
    FetchContent_Declare(
            libfsm
            GIT_REPOSITORY https://github.com/theo543/libfsm.git # fork of https://github.com/katef/libfsm
            GIT_TAG        62dfe789173d66e4f0816985ce0ba865b1ddb9f3 # ported to CMake
    )
    FetchContent_MakeAvailable(libfsm)
    target_compile_definitions(LFA_Tema_2 PRIVATE LIBFSM=1)
    target_include_directories(LFA_Tema_2 PRIVATE ${libfsm_SOURCE_DIR}/include)
    target_sources(LFA_Tema_2 PRIVATE src/libfsmWrapper.cpp src/libfsmWrapper.h)
    target_link_libraries(LFA_Tema_2 PRIVATE libfsm::libfsm)
else()
    message(STATUS "libfsm not enabled")
    target_compile_definitions(LFA_Tema_2 PRIVATE LIBFSM=0)
endif()

