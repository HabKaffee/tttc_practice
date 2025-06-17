cmake_minimum_required(VERSION 3.20)

project(LoopWrapperPass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include(AddLLVM)

llvm_map_components_to_libnames(llvm_libs support core)

add_library(LoopWrapperPass MODULE LoopWrapperPass.cpp)

set_target_properties(LoopWrapperPass PROPERTIES
  CXX_STANDARD 17
  PREFIX ""
  SUFFIX ".so"
)

target_link_libraries(LoopWrapperPass
  PRIVATE
    ${llvm_libs}
    clangFrontend
    clangAST
    clangBasic
)
