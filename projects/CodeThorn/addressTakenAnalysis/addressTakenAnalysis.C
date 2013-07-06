#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"
#include "Miscellaneous.h"

using namespace CodeThorn;

typedef std::set<VariableId> VariableIdSet;
typedef std::set<VariableId>::iterator VariableIdSetIterator;

class ExprTypeCycleDetect
{
  // pointer based '<' comparison
  // will do to detect if there is any cycle
  // we only go to type from SgExpression
  // NOTE: identify top of class hierarchy that defines get_type() interface
  // 
  std::set<SgNode*> visited;

public:
  ExprTypeCycleDetect() { }
  bool isVisited(SgNode* node)
  {
    if(visited.find(node) == visited.end())
      return false;
    return true;
  }

  void printVisitedSet()
  {
    std::set<SgNode*>::iterator it;
    std::cout << "<VisitedSet:\n";
    for(it = visited.begin(); it != visited.end(); it++)
    {
      std::cout << "<" << (*it)->unparseToString() << ", " << astTermToMultiLineString(*it) << "\n";
    }
    std::cout << ">\n";
  }

  bool operator()(SgNode* root)
  {
    // ignore the root
    RoseAst ast(root);
    for(RoseAst::iterator it = ast.begin(); it != ast.end(); it++)
    {
      if(isVisited(*it))
      {
        std::cout << "CYCLE!CYCLE!CYCLE!CYCLE!\n";
        return false;
      }

      visited.insert(*it);

      if(isSgExpression(*it))
      {
        // if already visited a expression -- cycle
        // std::cout << "Visit <" << *it << ", " << (*it)->unparseToString() << ">\n";

        SgType* type = isSgExpression(*it)->get_type();      
        RoseAst type_ast(type);

        for(RoseAst::iterator tIt = type_ast.begin(); tIt != type_ast.end(); tIt++)
        {
          // if its SgTypeDefType
          // skip the children
          if(isSgTypedefType(*tIt) || isSgEnumType(*tIt) || isSgClassType(*tIt))
            tIt.skipChildrenOnForward();

          // if already visited the children of a type
          if(isVisited(*tIt)) 
          {
            std::cout << "SUBTYPE: CYCLE!CYCLE!CYCLE!CYCLE!\n";            
            // write_file(type->class_name()+"_type_ast.dot", astTermToDot(type_ast.begin().withNullValues(), type_ast.end()));
            // write_file((*tIt)->class_name()+"_under_type_ast.dot", astTermWithNullValuesToDot(*tIt));
            // write_file((*it)->class_name() + "_ast.dot", astTermWithNullValuesToDot(*it));
            std::cout << "<Type AST: " << astTermToMultiLineString(type) << ">\n";
            std::cout << "<" << (*it)->unparseToString() << ", " << astTermToMultiLineString(*it) << ">\n";
            std::cout << "<" << (*tIt)->unparseToString() << ", " << astTermToMultiLineString(*tIt) << "\n";
            // printVisitedSet();
            return false;
          }
          // if not a typedefseq or SgType insert it to visited
          // if(!isSgTypedefSeq(*tIt) && !isSgType(*tIt))
          //   visited.insert(*tIt);
        }
      }
    }
    return true;
  }
};


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
  AstMatching m;
  match_result = m.performMatching(query, root);
  return match_result;
}

/*************************************************
 **************** OperandToVarID  ****************
 *************************************************/

// Visitor pattern to process the operands of SgAddressOfOp
// SgAddressOfOp can be applied to anything that is an r-value
// TODO: handle expressions that can be r-values
// 
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

  void visit(SgDotExp* sgn)
  {    
  }

  void visit(SgArrowExp* sgn)
  {
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
  // TODO: not sufficient to pick up address taken by function pointers
  MatchResult matches = procQuery("$HEAD=SgAddressOfOp($OP)", root);
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
 ************* PointerTypeAnalysis ***************
 *************************************************/
class PointerTypeAnalysis
{
  SgNode* root;
  VariableIdMapping& vidm;
  VariableIdSet pointerTypeSet;
public:
  PointerTypeAnalysis(SgNode* _root, VariableIdMapping& _vidm) : root(_root), vidm(_vidm) { }
  void collectPointerTypes();
};


void PointerTypeAnalysis::collectPointerTypes()
{
  // query AST
  ProcessQuery procQueryAST;
  // Note: do we need to do compound assign op for pointers ?
  MatchResult matches = procQueryAST("$X=SgAssignOp($L=SgVarRefExp,_,_)", root);
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); it++)
  {
    SgVarRefExp* lhs = (*it)["$L"];
    if(isSgPointerType(lhs->get_type()))
    {
      // lhs is a pointer variable
      std::cout << "<" << lhs->unparseToString() << "\n";
    }
  }

  matches.clear();
  matches = procQueryAST("$FP=SgFunctionParameterList");

}

/*************************************************
 ******************* main ************************
 *************************************************/
int main(int argc, char* argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  SgNode* root = project;

  RoseAst ast(root);

  // compute variableId mappings
  VariableIdMapping vidm;
  vidm.computeVariableSymbolMapping(project);

  AddressTakenAnalysis addrTakenAnalysis(root, vidm);
  addrTakenAnalysis.computeAddressTakenSet();
  addrTakenAnalysis.printAddressTakenSet();

  PointerTypeAnalysis ptrTypeAnalysis(root, vidm);
  ptrTypeAnalysis.collectPointerTypes();

  // ExprTypeCycleDetect etcd;
  // if(!etcd(root))
  //   return -1;

  return 0;
}
