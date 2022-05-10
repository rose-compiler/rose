#ifndef LLVM_ROSE_PASS_H                                        

#define LLVM_ROSE_PASS_H

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include "llvm/Analysis/DependenceAnalysis.h"

#include <rose.h> // POLICY_OK
#include "SgNodeHelper.h"

namespace RoseConnectionLLVMPassPlugin{

// New PM implementation
class ROSEPass : public llvm::PassInfoMixin<ROSEPass> {
  // Main entry point, takes IR unit to run the pass on (&M) and the
  // corresponding pass manager (to be queried if need be)
bool isDebugInfoAvail = false;


public:
  std::map<SgNode*, std::pair<int ,int >> ROSENodeMap;

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
  // get the map to record ROSE SgLocatedNode and src line/column
  std::map<SgNode*, std::pair<int ,int >> getRoseNodeInfo();
  // check if a SgLocatedNode has same line/column 
  bool matchROSESrcInfo(std::pair<int,int>);
  // get the line/column from the matched ROSE SgLocatedNode 
  std::pair<SgNode*, std::pair<int ,int >> getMatchingROSESrcInfo(std::pair<int,int>);
// get the operand information into a single string
  std::string getOperandInfo(llvm::Value* v, std::pair<int ,int > srcinfo);
  std::string getInstInfo(llvm::Instruction* i, std::pair<int ,int > srcinfo);
};


class nodeTraversal : public AstSimpleProcessing
{
  public:
      typedef std::map<SgNode*, std::pair<int ,int >> node_map_t; 
      const node_map_t  getNodeMap() const {return m;}

      virtual void visit(SgNode* n);
      void push_map_record(SgNode* node, std::pair<int ,int > srcInfo);
  private:
      node_map_t m;
};

}
#endif // LLVM_ROSE_PASS_H
