cmake_minimum_required (VERSION 2.6)
project (CppLox)
#set(CMAKE_C_COMPILER "/usr/bin/clang")
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Wextra -Werror -ggdb -D_GLIBCXX_DEBUG")
add_library(Scanner src/scanner.cpp)
add_library(Parser src/parser.cpp)
add_executable(CppLox main.cpp)
target_link_libraries(CppLox Scanner Parser)
add_custom_command(
    OUTPUT "${PROJECT_SOURCE_DIR}/src/ast.hpp"
    COMMAND perl6 ${PROJECT_SOURCE_DIR}/tools/GenerateAst.pl6 | clang-format | tee ${PROJECT_SOURCE_DIR}/src/ast.hpp
    MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/tools/GenerateAst.pl6"
)
add_custom_target(GenerateAst DEPENDS "${PROJECT_SOURCE_DIR}/src/ast.hpp")
add_dependencies(CppLox GenerateAst)
add_dependencies(Parser GenerateAst)
