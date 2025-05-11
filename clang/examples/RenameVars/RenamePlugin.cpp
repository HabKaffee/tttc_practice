#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace clang;

namespace {

class RenameVisitor : public RecursiveASTVisitor<RenameVisitor> {
public:
  RenameVisitor(Rewriter &R) : TheRewriter(R) {}

  bool VisitVarDecl(VarDecl *VD) {
    if (VD->isLocalVarDecl()) {
      if (VD->isStaticLocal()) {
        AddReplacement(VD, "static_");
      } else {
        AddReplacement(VD, "local_");
      }
    } else if (VD->hasGlobalStorage()) {
      AddReplacement(VD, "global_");
    }
    return true;
  }

  bool VisitParmVarDecl(ParmVarDecl *PVD) {
    AddReplacement(PVD, "param_");
    return true;
  }

  bool VisitDeclRefExpr(DeclRefExpr *DRE) {
    if (auto *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
      std::string OrigName = VD->getNameAsString();

      auto it = PrefixMap.find(OrigName);
      if (it == PrefixMap.end())
        return true;

      std::string NewName = it->second;
      SourceLocation Loc = DRE->getLocation();
      TheRewriter.ReplaceText(Loc, OrigName.length(), NewName);
    }
    return true;
  }

private:
  Rewriter &TheRewriter;
  std::map<std::string, std::string> PrefixMap;

  void AddReplacement(VarDecl *VD, const std::string &Prefix) {
    std::string OldName = VD->getNameAsString();
    std::string NewName = Prefix + OldName;
    PrefixMap[OldName] = NewName;

    SourceLocation Loc = VD->getLocation();
    TheRewriter.ReplaceText(Loc, OldName.length(), NewName);
  }
};

class RenameASTConsumer : public ASTConsumer {
public:
  RenameASTConsumer(Rewriter &R) : Visitor(R) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  RenameVisitor Visitor;
};

class RenameAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<RenameASTConsumer>(TheRewriter);
  }

  bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &) override {
    return true;
  }

  void EndSourceFileAction() override {
    TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID()).write(llvm::outs());
  }

private:
  Rewriter TheRewriter;
};

} // namespace

static FrontendPluginRegistry::Add<RenameAction>
    X("rename-vars", "Rename variables with proper prefixes");
