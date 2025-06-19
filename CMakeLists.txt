cmake_minimum_required(VERSION 3.13)
project(InlineFunctionPass)

find_package(LLVM REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(InlineFunctionPass MODULE InlineFunctionPass.cpp)

target_link_libraries(InlineFunctionPass ${LLVM_LIBRARIES})

set_target_properties(InlineFunctionPass PROPERTIES
  PREFIX ""
  SUFFIX ".so" 
 )