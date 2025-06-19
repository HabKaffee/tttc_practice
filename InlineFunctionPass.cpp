#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

namespace {
class InlineFunctionPass : public PassInfoMixin<InlineFunctionPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool modified = false;
    std::vector<CallInst *> callsToInline;

    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *callInst = dyn_cast<CallInst>(&I)) {
          Function *calledFunc = callInst->getCalledFunction();

          if (calledFunc && calledFunc->arg_empty() &&
              !calledFunc->isDeclaration()) {
            callsToInline.push_back(callInst);
          }
        }
      }
    }

    for (auto *callInst : callsToInline) {
      Function *calledFunc = callInst->getCalledFunction();
      ValueToValueMapTy VMap;
      SmallVector<ReturnInst *, 8> Returns;
      CloneFunctionInto(&F, calledFunc, VMap,
                        CloneFunctionChangeType::LocalChangesOnly, Returns);

      callInst->eraseFromParent();
      modified = true;
    }

    return modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK void LLVMInitializeInlineFunctionPass() {
  PassBuilder PB;
  PB.registerPipelineParsingCallback(
      [](StringRef Name, FunctionPassManager &PM,
         ArrayRef<PassBuilder::PipelineElement>) {
        if (Name == "inline-func") {
          PM.addPass(InlineFunctionPass());
          return true;
        }
        return false;
      });
}
/*
llvm::PassPluginLibraryInfo getInlineFunctionPassPluginInfo() {
    return { LLVM_PLUGIN_API_VERSION, "InlineFunctionPass", LLVM_VERSION_STRING,
            [](PassBuilder& PB) {
              PB.registerPipelineParsingCallback(
                  [](StringRef Name, FunctionPassManager& FPM,
                     ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "inline-func") {
                      FPM.addPass(InlineFunctionPass());
                      return true;
                    }
                    return false;
                  });
            } };
}

extern "C" LLVM_ATTRIBUTE_WEAK::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getInlineFunctionPassPluginInfo();
}*/