#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/LoopAnalysisManager.h"

using namespace llvm;

namespace {

class WrapLoopsPass : public PassInfoMixin<WrapLoopsPass> {
public:
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        // Получаем или объявляем функции loop_start и loop_end
        FunctionCallee loopStartFunc = M.getOrInsertFunction(
            "loop_start", Type::getVoidTy(M.getContext()));
        
        FunctionCallee loopEndFunc = M.getOrInsertFunction(
            "loop_end", Type::getVoidTy(M.getContext()));

        for (Function &F : M) {
            if (F.isDeclaration()) continue;
            
            FunctionAnalysisManager &FAM = 
                AM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
            
            LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
            
            for (Loop *L : LI) {
                errs() << "Found loop in function: " << F.getName() << "\n";
                
                // Получаем заголовок цикла и выходные блоки
                BasicBlock *header = L->getHeader();
                SmallVector<BasicBlock*, 4> exitBlocks;
                L->getExitBlocks(exitBlocks);
                
                // Вставляем loop_start в начале заголовка цикла
                IRBuilder<> builderStart(header, header->getFirstInsertionPt());
                builderStart.CreateCall(loopStartFunc);
                
                // Вставляем loop_end в каждом выходном блоке
                for (BasicBlock *exitBlock : exitBlocks) {
                    IRBuilder<> builderEnd(exitBlock, exitBlock->getFirstInsertionPt());
                    builderEnd.CreateCall(loopEndFunc);
                }
            }
        }
        
        return PreservedAnalyses::none(); // Мы изменили модуль
    }
};
}

llvm::PassPluginLibraryInfo getWrapLoopsPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "WrapLoopsPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "loop-wrapper") {
                        MPM.addPass(WrapLoopsPass());
                        return true;
                    }
                    return false;
                });
        }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getWrapLoopsPassPluginInfo();
}