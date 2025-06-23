#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

struct MyInlinePass : public PassInfoMixin<MyInlinePass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        std::vector<CallInst*> CallsToInline;
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (auto *Call = dyn_cast<CallInst>(&I)) {
                    Function *Callee = Call->getCalledFunction();
                    if (Callee && Callee->arg_empty() && Callee->getReturnType()->isVoidTy()) {
                        CallsToInline.push_back(Call);
                        
                    }
                }
            }
        }
        bool Changed = false;
        for (auto *Call : CallsToInline) {
            InlineFunctionInfo IFI;
            if (InlineFunction(*Call, IFI).isSuccess()) {
                Changed = true;
            }
        }
        return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }


};

llvm::PassPluginLibraryInfo getMyInlinePassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "MyInlinePass", "v0.1",
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager &FPM,
                        ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "my-inline-pass") {
                            FPM.addPass(MyInlinePass());
                            return true;
                        }
                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMyInlinePassPluginInfo();
}