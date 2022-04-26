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



#include "rose.h"
#include "SgNodeHelper.h"
using namespace llvm;

namespace RoseConnectionLLVMPassPlugin{

// New PM implementation
class ROSEPass : public PassInfoMixin<ROSEPass> {
  // Main entry point, takes IR unit to run the pass on (&M) and the
  // corresponding pass manager (to be queried if need be)
bool isDebugInfoAvail = false;


public:
  std::map<SgNode*, std::pair<int ,int >> ROSENodeMap;

  // run the pass on the module
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
  
  PreservedAnalyses runOnFunction(Function &F, FunctionAnalysisManager &FAM);

  PreservedAnalyses runOnGVariable(GlobalVariable &G);

  // check if debug option, -g, is given
  void checkCompiledWithDebugInfo(const Module& M);
  bool hasDebugInfo(){return isDebugInfoAvail;};

  // determine the dependence analysis report  
  std::string getDAResult(std::unique_ptr< Dependence >& result) const;
  // get the aliias result in string output
  std::string getAliasResult(AliasResult::Kind kind) const ;
  // get the map to record ROSE SgLocatedNode and src line/column
  std::map<SgNode*, std::pair<int ,int >> getRoseNodeInfo();
  // check if a SgLocatedNode has same line/column 
  bool matchROSESrcInfo(std::pair<int,int>);
  // get the line/column from the matched ROSE SgLocatedNode 
  std::pair<SgNode*, std::pair<int ,int >> getMatchingROSESrcInfo(std::pair<int,int>);
// get the operand information into a single string
  std::string getOperandInfo(Value* v, std::pair<int ,int > srcinfo);
  std::string getInstInfo(Instruction* i, std::pair<int ,int > srcinfo);
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
