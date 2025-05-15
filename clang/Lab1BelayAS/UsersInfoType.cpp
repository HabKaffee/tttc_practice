#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  bool VisitFunctionDecl(FunctionDecl *FD) {
    FD->dump(); // Выводим AST для функций
    return true;
  }
};

class MyASTConsumer : public ASTConsumer {
public:
  void HandleTranslationUnit(ASTContext &Context) override {
    visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  MyASTVisitor visitor;
};

class MyPluginAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(
    CompilerInstance &CI, StringRef InFile) {
    return std::make_unique<MyASTConsumer>();
  }

  bool ParseArgs(const CompilerInstance &CI,
                const std::vector<std::string> &args) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<MyPluginAction>
X("my-plugin", "My Clang Plugin");
