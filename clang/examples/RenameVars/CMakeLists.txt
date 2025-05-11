add_llvm_library(RenameVars MODULE RenamePlugin.cpp test1.c test2.c
  PLUGIN_TOOL clang
)

