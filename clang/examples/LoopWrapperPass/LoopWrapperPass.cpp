#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
struct LoopWrapperPass : PassInfoMixin<LoopWrapperPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
    LLVMContext &Ctx = F.getContext();
    Module *M = F.getParent();

    FunctionCallee LoopStart =
        M->getOrInsertFunction("loop_start", FunctionType::getVoidTy(Ctx));
    FunctionCallee LoopEnd =
        M->getOrInsertFunction("loop_end", FunctionType::getVoidTy(Ctx));

    for (Loop *L : LI)
      handleLoop(L, LoopStart, LoopEnd);

    return PreservedAnalyses::none();
  }

  void handleLoop(Loop *L, FunctionCallee &LoopStart, FunctionCallee &LoopEnd) {
    BasicBlock *Header = L->getHeader();
    IRBuilder<> Builder(Header->getFirstNonPHI());
    Builder.CreateCall(LoopStart);

    SmallVector<BasicBlock *, 8> ExitBlocks;
    L->getExitBlocks(ExitBlocks);
    for (BasicBlock *Exit : ExitBlocks) {
      IRBuilder<> Builder(Exit->getFirstNonPHI());
      Builder.CreateCall(LoopEnd);
    }

    for (Loop *SubLoop : L->getSubLoops())
      handleLoop(SubLoop, LoopStart, LoopEnd);
  }
};
} // namespace

llvm::PassPluginLibraryInfo getLoopWrapperPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopWrapperPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "loop-wrapper") {
                    FPM.addPass(LoopWrapperPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLoopWrapperPassPluginInfo();
}
