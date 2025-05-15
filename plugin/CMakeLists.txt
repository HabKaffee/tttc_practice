cmake_minimum_required(VERSION 3.10)
project(OutputofUserData)

find_package(Clang REQUIRED CONFIG)

add_library(OutputofUserData MODULE OutputofUserData.cpp)

target_include_directories(OutputofUserData PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(OutputofUserData PRIVATE clang)


set_target_properties(OutputofUserData PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)
