cmake_minimum_required(VERSION 3.12)
project(L)

set(CMAKE_CXX_STANDARD 20)

include_directories(./include)
include_directories(fmt/include)

add_subdirectory(fmt)

add_executable(L main.cpp src/lex.cpp include/lex.h src/parser.cpp include/parser.h include/ir.hpp src/vm.cpp include/vm.hpp include/codegen.hpp include/stdlib.hpp include/func.hpp include/type.hpp)
target_link_libraries(L dyncall_s fmt)