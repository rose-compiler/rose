#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"

using namespace CodeThorn;

typedef std::set<VariableId> VariableIdSet;
typedef std::set<VariableId>::iterator VariableIdSetIterator;

// AST Query Processor
// common functor to process any query and build match result
// NOTE: extend it to accept a functor to apply on each element of match result

/*************************************************
 ***************** ProcessQuery  *****************
 *************************************************/
class ProcessQuery
{
  // the root node on which the AST matching needs to be performed
  MatchResult match_result;
  AstMatching m;
public:
  ProcessQuery() { }
  // functor to operate on the given query
  MatchResult operator()(std::string query, SgNode* root);
  MatchResult getMatchResult();
  void printMatchResult();
  void clearMatchResult();
};

MatchResult ProcessQuery::getMatchResult()
{
  return match_result;
}

void ProcessQuery::printMatchResult()
{
  // MatchResult is list of maps
  // each map corresponds to one particular match instance
  for(MatchResult::iterator it = match_result.begin(); it != match_result.end(); it++)
  {
    for(SingleMatchVarBindings::iterator smbIt = (*it).begin(); smbIt != (*it).end(); smbIt++)
    {
      std::cout << "MATCH=";
      SgNode* matchedTerm = (*smbIt).second;
      ROSE_ASSERT(matchedTerm != NULL);
      std::cout << "  VAR: " << (*smbIt).first << "=" << astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
  }
}

void ProcessQuery::clearMatchResult()
{
  match_result.clear();
}

MatchResult ProcessQuery::operator()(std::string query, SgNode* root)
{
  match_result = m.performMatching(query, root);
  return match_result;
}

/*************************************************
 **************** OperandToVarID  ****************
 *************************************************/

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

/*************************************************
 ************** AddressTakenAnalysis  ************
 *************************************************/
class AddressTakenAnalysis
{
  // head of the AST on which the analysis should be performed
  SgNode* root;
  // result to be computed by this analysis
  VariableIdSet addressTakenSet;
  // required to compute VariableId
  VariableIdMapping& vidMapping;
public:
  AddressTakenAnalysis(SgNode* _root, VariableIdMapping& _vidMapping) : root(_root), vidMapping(_vidMapping) { }
  void computeAddressTakenSet();
  void printAddressTakenSet();
};

void AddressTakenAnalysis::computeAddressTakenSet()
{
  // query to match all SgAddressOfOp subtrees
  // process query
  ProcessQuery procQuery;
  MatchResult matches = procQuery("$HEAD=SgAddressOfOp($OP)", root);
  // procQuery.printMatchResult(); 
  OperandToVarID opToVarId(addressTakenSet, vidMapping);
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); it++)
  {
    SgNode* matchedOperand = (*it)["$OP"];
    matchedOperand->accept(opToVarId);
  }
}

// pretty print
void AddressTakenAnalysis::printAddressTakenSet()
{
  VariableIdSetIterator vidsIt = addressTakenSet.begin();
  std::cout << "addressTakenSet: [";
  for( ; vidsIt != addressTakenSet.end(); )
  {
    std::cout << vidMapping.variableName(*vidsIt);
    vidsIt++;
    if(vidsIt != addressTakenSet.end())
      std::cout << ", ";
  }
  std::cout << "]\n";  
}

/*************************************************
 ******************* main ************************
 *************************************************/
int main(int argc, char* argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  SgNode* root = project;

  // compute variableId mappings
  VariableIdMapping vidm;
  vidm.computeVariableSymbolMapping(project);

  AddressTakenAnalysis addrTakenAnalysis(root, vidm);
  addrTakenAnalysis.computeAddressTakenSet();
  addrTakenAnalysis.printAddressTakenSet();

  return 0;
}
