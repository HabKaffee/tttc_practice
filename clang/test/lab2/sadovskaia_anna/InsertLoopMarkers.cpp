#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/LegacyPassManager.h"

using namespace llvm;

namespace {
struct InsertLoopMarkers : public FunctionPass {
    static char ID;
    InsertLoopMarkers() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
        bool Changed = false;
        auto &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
        Module *M = F.getParent();

        LLVMContext &Ctx = M->getContext();
        IRBuilder<> Builder(Ctx);

        FunctionCallee loopStartFn = M->getOrInsertFunction("loop_start", FunctionType::get(Type::getVoidTy(Ctx), false));
        FunctionCallee loopEndFn = M->getOrInsertFunction("loop_end", FunctionType::get(Type::getVoidTy(Ctx), false));

        for (Loop *L : LI) {
            Changed |= processLoop(L, loopStartFn, loopEndFn);
        }

        return Changed;
    }

    bool processLoop(Loop *L, FunctionCallee loopStartFn, FunctionCallee loopEndFn) {
        bool Changed = false;

        BasicBlock *Header = L->getHeader();
        IRBuilder<> Builder(&*Header->getFirstInsertionPt());
        Builder.CreateCall(loopStartFn);

        SmallVector<BasicBlock *, 4> ExitBlocks;
        L->getExitBlocks(ExitBlocks);

        for (BasicBlock *ExitBB : ExitBlocks) {
            Instruction *FirstInstr = &*ExitBB->getFirstInsertionPt();
            IRBuilder<> ExitBuilder(FirstInstr);
            ExitBuilder.CreateCall(loopEndFn);
        }

        for (Loop *SubLoop : L->getSubLoops()) {
            Changed |= processLoop(SubLoop, loopStartFn, loopEndFn);
        }

        return true;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
        AU.addRequired<LoopInfoWrapperPass>();
    }
};
}

char InsertLoopMarkers::ID = 0;
static RegisterPass<InsertLoopMarkers> X("insert-loop-markers", "Insert loop_start()/loop_end() calls around loops");

