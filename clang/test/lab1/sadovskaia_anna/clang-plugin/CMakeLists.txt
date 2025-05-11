cmake_minimum_required(VERSION 3.10)
project(ImplicitCastPlugin)

find_package(Clang REQUIRED CONFIG)

include_directories(${CLANG_INCLUDE_DIRS})
link_directories(${CLANG_LIBRARY_DIRS})

add_library(ImplicitCastCounter MODULE ImplicitCastCounter.cpp)

target_link_libraries(ImplicitCastCounter
  clangAST
  clangFrontend
  clangASTMatchers
)

