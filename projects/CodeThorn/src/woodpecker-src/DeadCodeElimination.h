#include <list>
#include <set>

#include "VariableIdMapping.h"
#include "FIConstAnalysis.h"

class DeadCodeElimination {
 public:
  DeadCodeElimination();
  void setDetailedOutput(bool verbose);

  int eliminateDeadCodePhase1(SgNode* root,
                              VariableIdMapping* variableIdMapping,
                              VariableConstInfo& vci);
  void setVariablesOfInterest(VariableIdMapping::VariableIdSet& vidSet);

  std::set<SgFunctionDefinition*> NonCalledTrivialFunctions(SgNode* root0);
  void eliminateFunctions(std::set<SgFunctionDefinition*>& funDefs);

  std::list<SgIfStmt*> listOfEmptyIfStmts(SgNode* node);
  size_t eliminateNonCalledTrivialFunctions(SgNode* root);
  size_t eliminateEmptyIfStmts(SgNode* node);
  int numElimVars();
  int numElimAssignments();
  int numElimVarUses();
  int numElimFunctions();
 private:
  bool isVariableOfInterest(VariableId varId);
  bool isEmptyBlock(SgNode* node);
  bool isEmptyIfStmt(SgIfStmt* ifStmt);

  std::set<SgFunctionDefinition*> calledFunctionDefinitions(std::list<SgFunctionCallExp*> functionCalls);

  bool detailedOutput;
  VariableIdMapping::VariableIdSet variablesOfInterest;

  // statistics
  int elimVar;
  int elimAssignment;
  int elimVarUses;
  int elimFunctions;
};
