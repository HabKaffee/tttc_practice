#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include <set>

using namespace clang;

namespace {

class MaybeUnusedVisitor : public RecursiveASTVisitor<MaybeUnusedVisitor> {
public:
    MaybeUnusedVisitor(ASTContext &Context, Rewriter &R) 
        : Context(Context), Rewrite(R) {}

    bool VisitVarDecl(VarDecl *VD) {
        if (VD->isUsed() || VD->isReferenced()) return true;
        if (declVisited.find(VD) != declVisited.end()) return true;
        
        
        addMaybeUnusedAttr(VD);
        declVisited.insert(VD);
        return true;
    }

    bool VisitParmVarDecl(ParmVarDecl *PVD) {
        if (PVD->isUsed() || PVD->isReferenced()) return true;
        if (declVisited.find(PVD) != declVisited.end()) return true;
        
        addMaybeUnusedAttr(PVD);
        declVisited.insert(PVD);
        return true;
    }

private:
    ASTContext &Context;
    Rewriter &Rewrite;
    std::set<Decl*> declVisited;

    void addMaybeUnusedAttr(Decl *D) {
        SourceLocation Loc = D->getBeginLoc();
        if (Loc.isValid()) {
            Rewrite.InsertText(Loc, "[[maybe_unused]] ", true, true);
            llvm::errs() << "Added [[maybe_unused]] to " 
                         << cast<NamedDecl>(D)->getName() << "\n";
        }
    }
};

class MaybeUnusedConsumer : public ASTConsumer {
public:
    MaybeUnusedConsumer(CompilerInstance &CI) 
        : Rewrite(CI.getSourceManager(), CI.getLangOpts()),
          Visitor(CI.getASTContext(), Rewrite) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());

        const RewriteBuffer *Buf = Rewrite.getRewriteBufferFor(
            Context.getSourceManager().getMainFileID());
        if (Buf) {
            std::error_code EC;
            llvm::raw_fd_ostream OS(
                Rewrite.getSourceMgr().getFileEntryForID(
                    Rewrite.getSourceMgr().getMainFileID())->getName(),
                EC);
            Rewrite.getEditBuffer(
                Rewrite.getSourceMgr().getMainFileID()).write(OS);
            OS.close();
        }
    }

private:
    Rewriter Rewrite;
    MaybeUnusedVisitor Visitor;
};

class MaybeUnusedAction : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, StringRef InFile) override {
        return std::make_unique<MaybeUnusedConsumer>(CI);
    }

    bool ParseArgs(const CompilerInstance &CI,
                  const std::vector<std::string> &Args) override {
        return true;
    }

    PluginASTAction::ActionType getActionType() override {
        return AddBeforeMainAction;
    }
};

} // namespace

static FrontendPluginRegistry::Add<MaybeUnusedAction>
    X("maybe-unused", "Add [[maybe_unused]] to unused variables and parameters");