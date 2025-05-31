#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

namespace {

class CountInstructionsPass : public PassInfoMixin<CountInstructionsPass> {
public:
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        size_t InstructionCount = 0;

        for (Function &F : M) {
            if (F.isDeclaration()) continue;

            for (BasicBlock &BB : F) {
                for (Instruction &I : BB) {
                    ++InstructionCount;
                }
            }
        }

        errs() << "Total instruction count: " << InstructionCount << "\n";
        return PreservedAnalyses::all();
    }
};

} // namespace

// Pass registration for `opt` tool
llvm::PassPluginLibraryInfo getCountInstructionsPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "CountInstructionsPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "count-inst") {
                        MPM.addPass(CountInstructionsPass());
                        return true;
                    }
                    return false;
                });
        }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getCountInstructionsPassPluginInfo();
}
