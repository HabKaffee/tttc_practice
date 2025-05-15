#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include <map>

using namespace clang;

class ImplicitCastVisitor : public RecursiveASTVisitor<ImplicitCastVisitor> {
public:
    explicit ImplicitCastVisitor(ASTContext &Context) : Context(Context) {}
    
    bool TraverseImplicitCastExpr(ImplicitCastExpr *expr) {

        RecursiveASTVisitor<ImplicitCastVisitor>::TraverseImplicitCastExpr(expr);
        
        QualType sourceType = expr->getSubExpr()->getType();
        QualType targetType = expr->getType();
        
        if (!FunctionStack.empty()) {
            std::string from = sourceType.getAsString();
            std::string to = targetType.getAsString();
            castsMap[FunctionStack.back()][std::make_pair(from, to)]++;
        }
        
        return true;
    }
    
    bool TraverseFunctionDecl(FunctionDecl *func) {
        FunctionStack.push_back(func);
        RecursiveASTVisitor<ImplicitCastVisitor>::TraverseFunctionDecl(func);
        FunctionStack.pop_back();
        return true;
    }
    
    void printStats() {
        for (auto &funcEntry : castsMap) {
            llvm::outs() << "Function `" << funcEntry.first->getNameAsString() << "`\n";
            for (auto &castEntry : funcEntry.second) {
                llvm::outs() << castEntry.first.first << " -> " 
                            << castEntry.first.second << ": " 
                            << castEntry.second << "\n";
            }
            llvm::outs() << "\n";
        }
    }

private:
    ASTContext &Context;
    std::vector<FunctionDecl*> FunctionStack;
    std::map<FunctionDecl*, std::map<std::pair<std::string, std::string>, int>> castsMap;
};

class ImplicitCastConsumer : public ASTConsumer {
public:
    explicit ImplicitCastConsumer(ASTContext &Context) : Visitor(Context) {}
    
    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        Visitor.printStats();
    }

private:
    ImplicitCastVisitor Visitor;
};

class ImplicitCastPlugin : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) override {
        return std::make_unique<ImplicitCastConsumer>(CI.getASTContext());
    }
    
    bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &args) override {
        return true;
    }
};

static FrontendPluginRegistry::Add<ImplicitCastPlugin>
    X("lab1-plugin", "Counts implicit type conversions");
