#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"
#include "Miscellaneous.h"

using namespace CodeThorn;

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

// wrapper for set<VariableId> with pretty printing
// 
class VariableIdSet
{
  // required to get the variable name for the VariableId
  VariableIdMapping& vidm;
  std::set<VariableId> vset;
public:
  VariableIdSet(VariableIdMapping& _vidm) : vidm(_vidm) { }
  VariableIdSet(VariableIdMapping& _vidm, std::set<VariableId> _vset) : vidm(_vidm), vset(_vset) { }

  void insert(VariableId _vid)
  {
    vset.insert(_vid);
  }

  std::string str() const
  {
    std::ostringstream ostr;
    ostr << "[";
    std::set<VariableId>::iterator it = vset.begin();
    for( ; it != vset.end(); )
    {
      ostr << "<" << (*it).toString() << ", " << vidm.variableName(*it)  << ">";
      it++;
      if(it != vset.end())
        ostr << ", ";
    }
    ostr << "]";
    return ostr.str();
  }
};

/*************************************************
 ************** AddressTakenAnalysis  ************
 *************************************************/
class AddressTakenAnalysis
{
  // head of the AST on which the analysis should be performed
  SgNode* root;
  // required to compute VariableId
  VariableIdMapping& vidm;
  // result to be computed by this analysis
  VariableIdSet addressTakenSet;

  // address can be taken for any expression that is lvalue
  // The purpose of this class is to traverse arbitrary
  // expressions that are operands of SgAddressOfOp and find the
  // variable whose address is actually taken.
  // For example in expression &(a.b->c),  'c' address is
  // actually taken. This class simply traverses the rhs_operand
  // of SgDotExp/SgArrowExp or other expressions to identify 
  // the variable whose address is taken
  // 
  class OperandToVariableId : public ROSE_VisitorPatternDefaultBase
  {
    AddressTakenAnalysis& ata;
  public:
    OperandToVariableId(AddressTakenAnalysis& _ata) : ata(_ata) { }
    void visit(SgVarRefExp*);
    void visit(SgDotExp*);
    void visit(SgArrowExp*);
    void visit(SgPointerDerefExp*);
    void visit(SgPntrArrRefExp*);
    void visit(SgNode* sgn);
  };
public:
  AddressTakenAnalysis(SgNode* _root, VariableIdMapping& _vidm) : root(_root), vidm(_vidm), addressTakenSet(_vidm) { }
  void computeAddressTakenSet();
  void printAddressTakenSet();
};

void AddressTakenAnalysis::OperandToVariableId::visit(SgVarRefExp *sgn)
{  
  ata.addressTakenSet.insert(ata.vidm.variableId(sgn));
}

void AddressTakenAnalysis::OperandToVariableId::visit(SgDotExp* sgn)
{
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

void AddressTakenAnalysis::OperandToVariableId::visit(SgArrowExp* sgn)
{
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// For example q = &(*p) where both q and p are pointer types
// In the example, q can potentially modify all variables pointed to by p
// same as writing q = p.
// Since we dont know anything about p, q can potentially modify all 
// the elements in addressTakenSet as result of the above expression 
// As a result, the variables whose addresses can be taken is the entire 
// set as we dont have any idea about p
// We dont need to add any new variable to addressTakenSet 
// as a consequence of the expressions similar to above.
void AddressTakenAnalysis::OperandToVariableId::visit(SgPointerDerefExp* sgn)
{
  // we dont do anything here
}

// For example &(A[B[C[..]]])
// any pointer can that takes this address can modify
// contents of only A. The inner index expressions are r-values
// it is sufficient to add A to addressTakenSet
// 
void AddressTakenAnalysis::OperandToVariableId::visit(SgPntrArrRefExp* sgn)
{
  SgNode* arr_op = sgn->get_lhs_operand();
  SgVarRefExp* arr_name = isSgVarRefExp(arr_op); ROSE_ASSERT(arr_name != NULL);
  ata.addressTakenSet.insert(ata.vidm.variableId(arr_name));
}

void AddressTakenAnalysis::OperandToVariableId::visit(SgNode* sgn)
{
  std::cerr << "unhandled operand of SgAddressOfOp in AddressTakenAnalysis\n";
  ROSE_ASSERT(0);
}

void AddressTakenAnalysis::computeAddressTakenSet()
{
  // query to match all SgAddressOfOp subtrees
  // process query
  ProcessQuery procQuery;
  // TODO: not sufficient to pick up address taken by function pointers
  MatchResult matches = procQuery("$HEAD=SgAddressOfOp($OP)", root);
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); it++)
  {
    SgNode* matchedOperand = (*it)["$OP"];
    OperandToVariableId optovid(*this);
    matchedOperand->accept(optovid);
  }
}

// pretty print
void AddressTakenAnalysis::printAddressTakenSet()
{  
  std::cout << "addressTakenSet: " << addressTakenSet.str() << "\n";
}

/*************************************************
 **************** TypeAnalysis *******************
 *************************************************/
class TypeAnalysis
{
  VariableIdMapping& vidm;
  VariableIdSet pointerTypeSet;
  VariableIdSet arrayTypeSet;

public:
  TypeAnalysis(VariableIdMapping& _vidm) : vidm(_vidm), pointerTypeSet(_vidm), arrayTypeSet(_vidm) { }
  void collectTypes();
  void printPointerTypeSet();
  void printArrayTypeSet();
};


void TypeAnalysis::collectTypes()
{
  std::set<VariableId> set = vidm.getVariableIdSet();
  for(std::set<VariableId>::iterator it = set.begin(); it != set.end(); ++it)
  {
    SgSymbol* v_symbol = vidm.getSymbol(*it);
    SgType* v_type = v_symbol->get_type();
    // Note on function pointer types
    // function pointer can modify any variable
    // not just the variables in the addressTakenSet
    // answering function pointer derefence requires side
    // effect analysis to determine the list of variables
    // can modify. Currenty we ignore function pointers as
    // the goal of this analysis is supposed to be simple.
    if(isSgPointerType(v_type))
    {
      SgType* baseType = v_type->findBaseType();
      // perhaps its worthwile to keep them in
      // a separte set and not support any dereferencing
      // queries rather than not adding them
      if(!isSgFunctionType(baseType))
      {
        pointerTypeSet.insert(*it);
      }
    }
    else if(isSgArrayType(v_type))
    {
      arrayTypeSet.insert(*it);
    }
  }
}

void TypeAnalysis::printPointerTypeSet()
{
  std::cout << "pointerTypeSet: " << pointerTypeSet.str() << "\n";
}

void TypeAnalysis::printArrayTypeSet()
{
  std::cout << "arrayTypeSet: " << arrayTypeSet.str() << "\n";
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

  TypeAnalysis typeAnalysis(vidm);
  typeAnalysis.collectTypes();
  typeAnalysis.printPointerTypeSet();
  typeAnalysis.printArrayTypeSet();

  // ExprTypeCycleDetect etcd;
  // if(!etcd(root))
  //   return -1;

  return 0;
}
