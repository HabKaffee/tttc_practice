#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;

namespace {

class CastCounter : public MatchFinder::MatchCallback {
public:
    int Count = 0;

    void run(const MatchFinder::MatchResult &Result) override {
        if (const auto *Cast = Result.Nodes.getNodeAs<ImplicitCastExpr>("cast")) {
            ++Count;

            const SourceManager *SM = Result.SourceManager;
            SourceLocation Loc = Cast->getExprLoc();
            if (Loc.isValid() && SM) {
                PresumedLoc PLoc = SM->getPresumedLoc(Loc);
            }
        }
    }
};

class CastPlugin : public PluginASTAction {
    CastCounter Callback;
    MatchFinder Finder;

protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) override {
        Finder.addMatcher(implicitCastExpr().bind("cast"), &Callback);
        return Finder.newASTConsumer();
    }

    bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &args) override {
        return true;
    }

    void EndSourceFileAction() override {
        llvm::errs() << "Total implicit casts: " << Callback.Count << "\n";
    }
};

}

static FrontendPluginRegistry::Add<CastPlugin>
X("count-casts", "counting implicit conversions");

