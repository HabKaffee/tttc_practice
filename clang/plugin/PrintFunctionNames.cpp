#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "clang/Lex/Lexer.h"

using namespace clang;

class UnusedVarVisitor : public RecursiveASTVisitor<UnusedVarVisitor> {
public:
    explicit UnusedVarVisitor(ASTContext *Context) : Context(Context) {}

    bool VisitVarDecl(VarDecl *VD) {
        if (!VD->isUsed() && !VD->hasAttr<UnusedAttr>()) {
            SourceLocation Loc = VD->getLocation();
            SourceManager &SM = Context->getSourceManager();
            
            if (SM.isWrittenInMainFile(Loc)) {
                DiagnosticsEngine &Diag = Context->getDiagnostics();
                unsigned ID = Diag.getCustomDiagID(
                    DiagnosticsEngine::Warning,
                    "Variable %0 is unused, consider adding [[maybe_unused]]"
                );
                Diag.Report(Loc, ID) << VD->getName();
                
                // Добавляем атрибут
                VD->addAttr(UnusedAttr::CreateImplicit(*Context));
            }
        }
        return true;
    }

    bool VisitParmVarDecl(ParmVarDecl *PVD) {
        if (!PVD->isUsed() && !PVD->hasAttr<UnusedAttr>()) {
            SourceLocation Loc = PVD->getLocation();
            SourceManager &SM = Context->getSourceManager();
            
            if (SM.isWrittenInMainFile(Loc)) {
                PVD->addAttr(UnusedAttr::CreateImplicit(*Context));
            }
        }
        return true;
    }

private:
    ASTContext *Context;
};

class UnusedVarConsumer : public ASTConsumer {
public:
    explicit UnusedVarConsumer(ASTContext *Context) : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    UnusedVarVisitor Visitor;
};

class UnusedVarAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, llvm::StringRef) override {
        return std::make_unique<UnusedVarConsumer>(&CI.getASTContext());
    }

    bool ParseArgs(const CompilerInstance &CI,
                  const std::vector<std::string> &args) override {
        return true;
    }
};

static FrontendPluginRegistry::Add<UnusedVarAction>
    X("unused-var", "Adds [[maybe_unused]] to unused variables");