cmake_minimum_required(VERSION 3.22)
project(assert-cegar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(assertcegar ${SOURCES})

add_executable(dfa_tests tests/DFA_tests.cpp)
target_include_directories(dfa_tests PRIVATE src)

add_executable(lexer_tests tests/lexer_tests.cpp)
target_include_directories(lexer_tests PRIVATE src)

add_executable(parser_tests tests/parser_tests.cpp)
target_include_directories(parser_tests PRIVATE src)

enable_testing()
add_test(NAME dfa_tests    COMMAND dfa_tests)
add_test(NAME lexer_tests  COMMAND lexer_tests)
add_test(NAME parser_tests COMMAND parser_tests)