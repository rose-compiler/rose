//===- RoseConnectionClangPlugin.cpp ---------------------------------------------===//
//
// Example clang plugin which incooperate ROSE. 
//
//===----------------------------------------------------------------------===//
#include "RoseConnectionClangPlugin.hpp"

namespace {

class collectInfoVisitor : public RecursiveASTVisitor<collectInfoVisitor> {

public:
  explicit collectInfoVisitor(ASTContext *Context)
    : Context(Context) {}


private:
  ASTContext *Context;
};


class RoseConnectionClangPluginConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  llvm::StringRef inFile;

public:
  RoseConnectionClangPluginConsumer(CompilerInstance &Instance, llvm::StringRef inFile)
      : Instance(Instance), visitor(&Instance.getASTContext()), inFile(inFile) {}

  void Initialize(ASTContext &Context) override {
    SourceManager& SrcMgr = Context.getSourceManager();
    llvm::errs() << "my customized initizlier for input file: " << inFile << "\"\n";
    
    ROSE_INITIALIZE;
    std::vector<std::string> roseFEArgs{"","-rose:skipfinalCompileStep",inFile.str()};
    roseProject = ::frontend(roseFEArgs); 
    std::cout << "In Plugin: generated sgproject:" << roseProject << std::endl;

 }

 virtual void HandleTranslationUnit(clang::ASTContext &Context) override{
   visitor.TraverseDecl(Context.getTranslationUnitDecl());
 }

private:
  collectInfoVisitor visitor;
};

class RoseConnectionClangPluginAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef inFile) override {
    return std::make_unique<RoseConnectionClangPluginConsumer>(CI, inFile);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {

    // arguments are passed from command line by "-Xclang -plugin-arg-ROSE -Xclang $ARG" 

    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "RoseConnectionClangPlugin arg = " << args[i] << "\n";

      // Example error handling and argument processing.
      DiagnosticsEngine &D = CI.getDiagnostics();
      if (args[i] == "-an-error") {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "invalid argument '%0'");
        D.Report(DiagID) << args[i];
        return false;
      } else if (args[i] == "-parse-template") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -parse-template argument"));
          return false;
        }
        ++i;
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }


  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for RoseConnectionClangPlugin plugin goes here\n";
  }

  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }

};

}

static FrontendPluginRegistry::Add<RoseConnectionClangPluginAction>
X("ROSE", "call ROSE");
