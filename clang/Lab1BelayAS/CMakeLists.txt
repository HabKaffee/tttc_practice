cmake_minimum_required(VERSION 3.28)  # Понизили версию для совместимости
project(UsersInfoType)

set(LLVM_DIR "/mnt/d/compilers/tttc_practice/build_rel/lib/cmake/llvm")
set(Clang_DIR "/mnt/d/compilers/tttc_practice/build_rel/lib/cmake/clang")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(SYSTEM
  ${LLVM_INCLUDE_DIRS}
  ${CLANG_INCLUDE_DIRS}
)

include(AddLLVM)

add_llvm_library(UsersInfoType
  MODULE
  UsersInfoType.cpp
  PLUGIN_TOOL clang
)

