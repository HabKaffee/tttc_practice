cmake_minimum_required(VERSION 3.20)
project(LoopWrapperPass LANGUAGES C CXX)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include(AddLLVM)
add_llvm_library(LoopWrapperPass MODULE
  LoopWrapper.cpp
  PLUGIN_TOOL
  opt
)