#include "llvm/Passes/PassPlugin.h" 
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"


using namespace llvm;
namespace {
struct InstrumentationPass : public PassInfoMixin<InstrumentationPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
    LLVMContext &Context = M.getContext();
    FunctionCallee instrument_start_func = M.getOrInsertFunction(
        "instrument_start", FunctionType::getVoidTy(Context));
    FunctionCallee instrument_end_func = M.getOrInsertFunction(
        "instrument_end", FunctionType::getVoidTy(Context));

    for (Function &F : M) {
      if (F.isDeclaration()) {
        continue;
      }
      BasicBlock &entryBlock = F.getEntryBlock();
      IRBuilder<> builder(&entryBlock, entryBlock.begin());
      builder.CreateCall(instrument_start_func);
      for (BasicBlock &BB : F) {
        if (ReturnInst *retInst = dyn_cast<ReturnInst>(BB.getTerminator())) {
          IRBuilder<> builder(retInst);
          builder.CreateCall(instrument_end_func);
        }
      }
    }
    return PreservedAnalyses::all();
  }
};
llvm::PassPluginLibraryInfo getInstrumentationPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InstrumentationPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef name, llvm::ModulePassManager &PM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                    if (name == "instrumentation-pass") {
                    PM.addPass(InstrumentationPass{});
                    return true;
                  }
                  return false;
                });
          }};
}
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return getInstrumentationPassPluginInfo();
}
} 
