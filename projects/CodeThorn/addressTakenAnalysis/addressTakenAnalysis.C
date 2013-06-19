#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"

using namespace CodeThorn;

typedef std::set<VariableId> VariableIdSet;
typedef std::set<VariableId>::iterator VariableIdSetIterator;

// debug print
void printMatchResults(MatchResult& matches)
{
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); it++)
  {   
    for(SingleMatchVarBindings::iterator smvbIt = (*it).begin(); smvbIt != (*it).end(); smvbIt++)
    {
      std::string var = (*smvbIt).first;
      SgNode* matched = (*smvbIt).second;
      std::cout << "VAR=" << var << ", MATCHED=" << matched->unparseToString() << std::endl;
    }
  }
}

// Visitor pattern to process the operands of SgAddressOfOp
class OperandToVarID : public ROSE_VisitorPatternDefaultBase
{
  // store the reference to address taken set
  VariableIdSet& addressTakenSet;
  // store the reference to VariableId mappings
  VariableIdMapping& vidm;
  
public:
  OperandToVarID(VariableIdSet& _addrTakenSet, VariableIdMapping& _vidm) : addressTakenSet(_addrTakenSet), vidm(_vidm)  { }                                                                                                                                                                                                                                                    
  void visit(SgVarRefExp* sgn)
  {
    // get the VariableId for SgVarRefExp
    addressTakenSet.insert(vidm.variableId(sgn));
  }
};

int main(int argc, char* argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  SgNode* root = project;

  // compute variableId mappings
  VariableIdMapping vidm;
  vidm.computeVariableSymbolMapping(project);

  // set to track address taken elements
  VariableIdSet addressTakenSet;

  OperandToVarID optovarid(addressTakenSet, vidm);

  AstMatching m;
  // NOTE: $X=SgAddressOfOp($Y=_) should not be a parser error. Verify !!
  // query to match all SgAddressOfOp subtrees
  MatchResult matches = m.performMatching("$HEAD=SgAddressOfOp($OP)", root);

  // MatchResult is list of maps
  // each map corresponds to one particular match instance
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); it++)
  {
    SgNode* matchedOperand = (*it)["$OP"];
    ROSE_ASSERT(matchedOperand != NULL);
    std::cout << "MATCHED OP=" << matchedOperand->unparseToString() << " classname=" << matchedOperand->class_name() << std::endl;
    matchedOperand->accept(optovarid);  
  }

  VariableIdSetIterator vidsIt = addressTakenSet.begin();
  std::cout << "addressTakenSet: {";
  for( ; vidsIt != addressTakenSet.end(); vidsIt++)
  {
    std::cout << vidm.variableName(*vidsIt) << ",";
  }
  std::cout << "}\n";

  return 0;
}
