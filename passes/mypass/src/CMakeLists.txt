cmake_minimum_required(VERSION 3.13)
project(CountInstructionsPass LANGUAGES C CXX)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(CountInstructionsPass MODULE CountInstructions.cpp)

set_target_properties(CountInstructionsPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)