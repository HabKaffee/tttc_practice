#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  explicit MyASTVisitor(ASTContext &Context) : Context(Context) {}

  bool VisitCXXRecordDecl(CXXRecordDecl *Decl) {
    
    if (Decl->getName().empty() || !Decl->isCompleteDefinition()) {
      return true;
    }

    
    llvm::outs() << Decl->getName() << "\n";

    
    if (Decl->getNumBases() > 0) {
      llvm::outs() << "|_Base classes\n";
      for (const auto &Base : Decl->bases()) {
        const auto *BaseDecl = Base.getType()->getAsCXXRecordDecl();
        if (BaseDecl) {
          llvm::outs() << "| |_ " << BaseDecl->getName() 
                       << " (" << getAccessSpecifierString(Base.getAccessSpecifier()) << ")\n";
        }
      }
    }

    
    if (Decl->field_begin() != Decl->field_end()) {
      llvm::outs() << "|_Fields\n";
      for (const auto *Field : Decl->fields()) {
        llvm::outs() << "| |_ " << Field->getName() 
                     << " (" << Field->getType().getAsString()
                     << "|" << getAccessSpecifierString(Field->getAccess()) << ")\n";
      }
    }

    
    if (Decl->method_begin() != Decl->method_end()) {
      llvm::outs() << "|_Methods\n";
      for (const auto *Method : Decl->methods()) {
        
        if (Method->isImplicit() || Method->isOverloadedOperator()) {
          continue;
        }

        llvm::outs() << "| |_ " << Method->getNameAsString()
                     << " (" << Method->getReturnType().getAsString() << "()"
                     << "|" << getAccessSpecifierString(Method->getAccess());

        if (Method->isVirtual()) {
          llvm::outs() << "|virtual";
          if (Method->isPure()) {
            llvm::outs() << "|pure";
          }
        }
        if (Method->size_overridden_methods() > 0) {
          llvm::outs() << "|override";
        }

        llvm::outs() << ")\n";
      }
    }

    llvm::outs() << "\n";
    return true;
  }

private:
  ASTContext &Context;

  static const char *getAccessSpecifierString(AccessSpecifier AS) {
    switch (AS) {
      case AS_public:    return "public";
      case AS_protected: return "protected";
      case AS_private:   return "private";
      case AS_none:      return "none";
    }
    llvm_unreachable("Unknown access specifier");
  }
};

class MyASTConsumer : public ASTConsumer {
public:
  explicit MyASTConsumer(ASTContext &Context) : Visitor(Context) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  MyASTVisitor Visitor;
};

class MyPluginAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                               StringRef InFile) override {
    return std::make_unique<MyASTConsumer>(CI.getASTContext());
  }

  bool ParseArgs(const CompilerInstance &CI,
                const std::vector<std::string> &Args) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<MyPluginAction>
X("class-info", "Prints information about classes, structs and their members");