#ifndef ROSE_CONNECTION_LLVM_PASS_PLUGIN_H                                        

#define ROSE_CONNECTION_LLVM_PASS_PLUGIN_H

#include "clang/Basic/Version.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Support/JSON.h"
#include <llvm/Support/FileSystem.h>



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
namespace RoseConnectionLLVMPassPlugin{

// New PM implementation
class ROSEPass : public llvm::PassInfoMixin<ROSEPass> {
  // Main entry point, takes IR unit to run the pass on (&M) and the
  // corresponding pass manager (to be queried if need be)
bool isDebugInfoAvail = false;


public:
  using InstInfo = std::pair<llvm::Instruction*, std::pair<int,int>>;

  class depInfo{
    public:
      InstInfo inst1;
      InstInfo inst2;
      std::string depType;    
      depInfo(InstInfo i1, InstInfo i2, std::string dep) : inst1(i1), inst2(i2), depType(dep){}

  };

  std::vector<depInfo> depSet; 

  // run the pass on the module
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM);
  
  llvm::PreservedAnalyses runOnFunction(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);

  llvm::PreservedAnalyses runOnGVariable(llvm::GlobalVariable &G);

  // check if debug option, -g, is given
  void checkCompiledWithDebugInfo(const llvm::Module& M);
  bool hasDebugInfo(){return isDebugInfoAvail;};

  // determine the dependence analysis report  
  std::string getDAResult(std::unique_ptr< llvm::Dependence >& result) const;
  // get the aliias result in string output
  std::string getAliasResult(llvm::AliasResult::Kind kind) const ;
// get the operand information into a single string
  std::string getOperandInfo(llvm::Value* v, std::pair<int ,int > srcinfo);
  std::string getInstInfo(llvm::Instruction* i, std::pair<int ,int > srcinfo);
};


class SarifDiagnostics  {
  std::string OutputFile;

public:
  SarifDiagnostics(const std::string &Output)
      : OutputFile(Output) {}
  ~SarifDiagnostics() = default;

  void FlushDiagnosticsImpl(std::vector<ROSEPass::depInfo>& depSet) ;

  llvm::StringRef getName() const  { return "SarifDiagnostics"; }
};


}
#endif // ROSE_CONNECTION_LLVM_PASS_PLUGIN_H
