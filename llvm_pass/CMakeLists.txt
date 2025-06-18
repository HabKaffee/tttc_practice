cmake_minimum_required(VERSION 3.10)
project(InstrumentationPass)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

add_library(InstrumentationPass MODULE InstrumentationPass.cpp)

target_include_directories(InstrumentationPass PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(InstrumentationPass PRIVATE clang)

