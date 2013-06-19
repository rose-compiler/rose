#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"

using namespace CodeThorn;

typedef std::set<VariableId> VariableIdSet;

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

class ProcessOperand : public ROSE_VisitorPatternDefaultBase
{
  // store the reference to address taken set
  VariableIdSet& addressTakenSet;
  VariableIdMapping vidm;
  
public:
  ProcessOperand(VariableIdSet& _addrTakenSet) : addressTakenSet(_addrTakenSet) { }                                                                                                                                                                                                                                                    
  void visit(SgVarRefExp* sgn)
  {
    VariableId varID = vidm.variableId(sgn);
    // push it into the set
    addressTakenSet.insert(varID);
  }
};

int main(int argc, char* argv[])
{
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);
  SgNode* root = sageProject;

  VariableIdSet addressTakenSet;
  ProcessOperand processoperand(addressTakenSet);

  AstMatching m;
  // NOTE: $X=SgAddressOfOp($Y=_) should not be a parser error. Verify !!
  MatchResult matches = m.performMatching("$HEAD=SgAddressOfOp($OP)", root);
  // MatchResult is list of maps
  // each map corresponds to one particular match instance
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); it++)
  {
    SgNode* matchedOperand = (*it)["$OP"];
    ROSE_ASSERT(matchedOperand != NULL);
    std::cout << "MATCHED OP=" << matchedOperand->unparseToString() << std::endl;
    
  }
  
  return 0;
}
