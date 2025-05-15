cmake_minimum_required(VERSION 3.20)
project(UnusedVarPlugin)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS} ${CLANG_DEFINITIONS})

add_compile_options(-fno-rtti -fPIC)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,dynamic_lookup")

add_library(PrintFunctionNames MODULE
  PrintFunctionNames.cpp
)

set_target_properties(PrintFunctionNames PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  PREFIX ""
  PLUGIN_TOOL "clang" 
)

target_link_libraries(PrintFunctionNames
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangTooling
  clangDriver  
  clangParse
  clangSerialization
)