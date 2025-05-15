cmake_minimum_required(VERSION 3.22)

project(MaybeUnusedPlugin)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(
  /usr/lib/llvm-17/include
  /usr/lib/llvm-17/tools/clang/include
)

link_directories(/usr/lib/llvm-17/lib)

add_library(MaybeUnusedPlugin MODULE MaybeUnusedPlugin.cpp)

set_target_properties(MaybeUnusedPlugin PROPERTIES
  CXX_STANDARD 17
  PREFIX ""
  SUFFIX ".so"
)

target_link_libraries(MaybeUnusedPlugin
  PRIVATE
    clangAST
    clangASTMatchers
    clangBasic
    clangFrontend
)