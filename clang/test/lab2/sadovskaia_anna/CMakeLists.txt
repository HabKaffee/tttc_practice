cmake_minimum_required(VERSION 3.13)
project(InsertLoopMarkers)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(InsertLoopMarkers MODULE InsertLoopMarkers.cpp)

set_target_properties(InsertLoopMarkers PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)
