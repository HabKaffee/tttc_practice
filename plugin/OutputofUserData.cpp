#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class TypeInspector : public ASTConsumer {
    CompilerInstance &CI;
    std::string convertAccess(AccessSpecifier AS) const {
        switch (AS) {
          case AS_public: return "public";
          case AS_protected: return "protected";
          case AS_private: return "private";
          default: return "none";
    }
  }

    void showFields(const CXXRecordDecl *CXXDecl) {
        for (const FieldDecl *Field : CXXDecl->fields()) {
          llvm::outs() << "| |_ " << Field->getNameAsString() << " ("
                       << Field->getType().getAsString() << "|"
                       << convertAccess(Field->getAccess()) << ")\n";
    }
  }

    void showMethods(const CXXRecordDecl *CXXDecl) {
        for (const CXXMethodDecl *Method : CXXDecl->methods()) {
          if (Method->isImplicit()) continue;
          std::string flags;
          if (Method->isVirtual()) flags += "|virtual";
          if (Method->isPureVirtual()) flags += "|pure";
          if (Method->size_overridden_methods() > 0) flags += "|override";
          llvm::outs() << "| |_ " << Method->getNameAsString() << " ("
                       << Method->getReturnType().getAsString() << "()|"
                       << convertAccess(Method->getAccess()) << flags << ")\n";
    }
  }

    void displayClassInfo(const CXXRecordDecl *CXXDecl) {
        llvm::outs() << CXXDecl->getNameAsString();
        for (const auto &Base : CXXDecl->bases()) {
            if (const auto *BaseDecl = Base.getType()->getAsCXXRecordDecl()) {
                llvm::outs() << "->" << BaseDecl->getNameAsString();
      }
    }

    llvm::outs() << "\n|_Fields\n";
    showFields(CXXDecl);
    llvm::outs() << "|\n|_Methods\n";
    showMethods(CXXDecl);
    llvm::outs() << "\n";
  }

public:
    explicit TypeInspector(CompilerInstance &CI) : CI(CI) {}
  
    void HandleTranslationUnit(ASTContext &Ctx) override {
        for (Decl *DeclNode : Ctx.getTranslationUnitDecl()->decls()) {
            if (auto *CXXDecl = dyn_cast<CXXRecordDecl>(DeclNode)) {
                if (CXXDecl->isCompleteDefinition() && !CXXDecl->isImplicit()) {
                    displayClassInfo(CXXDecl);
        }
      }
    }
  }
};

class TypeInspectorAction : public PluginASTAction {
protected:
    bool ParseArgs(const CompilerInstance &, const std::vector<std::string> &) override {
        return true;
  }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                               llvm::StringRef) override {
        return std::make_unique<TypeInspector>(CI);
  }
};

static FrontendPluginRegistry::Add<TypeInspectorAction>
X("user-data", "Output of User Data");
