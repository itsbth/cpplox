cmake_minimum_required (VERSION 2.6)
project (CppLox)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -Werror -ggdb -D_GLIBCXX_DEBUG")

if(USE_CLANG)
    set(CMAKE_C_COMPILER "clang-4.0")
    set(CMAKE_CXX_COMPILER "clang++-4.0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

add_library(Scanner src/scanner.cpp)
add_library(Parser src/parser.cpp)
add_library(Interpreter src/interpreter.cpp)
add_executable(CppLox main.cpp)
target_link_libraries(CppLox Scanner Parser Interpreter)
add_custom_command(
    OUTPUT "${PROJECT_SOURCE_DIR}/src/ast.hpp"
    COMMAND perl6 ${PROJECT_SOURCE_DIR}/tools/GenerateAst.pl6 | clang-format | tee ${PROJECT_SOURCE_DIR}/src/ast.hpp
    MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/tools/GenerateAst.pl6"
)
add_custom_target(GenerateAst DEPENDS "${PROJECT_SOURCE_DIR}/src/ast.hpp")
add_dependencies(CppLox GenerateAst)
add_dependencies(Parser GenerateAst)
add_dependencies(Interpreter GenerateAst)
