#include "SIMDAnalysis.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace SIMDAnalysis;

/******************************************************************************************************************************/
/*
  Get the Def information.
  This is reserved for later usage.
*/
/******************************************************************************************************************************/

void SIMDAnalysis::getDefList(DFAnalysis* defuse, SgNode* root) 
{
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(root, V_SgVarRefExp); 
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  for (; i!=vars.end();++i) 
  {
    SgVarRefExp * varRef = isSgVarRefExp(*i);
    SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
    std::string name = initName->get_qualified_name().str();
    // Find reaching definition of initName at the control flow node varRef
    vector<SgNode* > vec = defuse->getDefFor(varRef, initName);
    ROSE_ASSERT (vec.size() >0 ); // each variable reference must have a definition somewhere
    
    // Output each reaching definition node and the corresponding statement.
    std::cout<<"---------------------------------------------"<<std::endl;
    std::cout << vec.size() << " Def entry/entries for " << varRef->unparseToString() <<  
    " @ line " << varRef->get_file_info()->get_line()<<":"<<varRef->get_file_info()->get_col() 
    << std::endl;
    for (size_t j =0; j<vec.size(); j++)
    {
      cout<<vec[j]->class_name()<<" "<<vec[j]<<endl;
      SgStatement * def_stmt = SageInterface::getEnclosingStatement(vec[j]);
      ROSE_ASSERT(def_stmt);
      cout<<def_stmt->unparseToString()<<"  @ line "<<def_stmt->get_file_info()->get_line()
        <<":"<<def_stmt->get_file_info()->get_col() <<endl;
    }
  }
}


/******************************************************************************************************************************/
/*
  Get the Use information.
  This is reserved for later usage.
*/
/******************************************************************************************************************************/
void SIMDAnalysis::getUseList(DFAnalysis* defuse, SgNode* root) 
{
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(root, V_SgVarRefExp); 
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  for (; i!=vars.end();++i) 
  {
    SgVarRefExp * varRef = isSgVarRefExp(*i);
    SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
    std::string name = initName->get_qualified_name().str();
    // Find reaching definition of initName at the control flow node varRef
    vector<SgNode* > vec = defuse->getUseFor(varRef, initName);
//    ROSE_ASSERT (vec.size() >0 ); // each variable reference must have a definition somewhere
    
    // Output each reaching definition node and the corresponding statement.
    std::cout<<"---------------------------------------------"<<std::endl;
    std::cout << vec.size() << " Use entry/entries for " << varRef->unparseToString() <<  
    " @ line " << varRef->get_file_info()->get_line()<<":"<<varRef->get_file_info()->get_col() 
    << std::endl;
    for (size_t j =0; j<vec.size(); j++)
    {
      cout<<vec[j]->class_name()<<" "<<vec[j]<<endl;
      SgStatement * def_stmt = SageInterface::getEnclosingStatement(vec[j]);
      ROSE_ASSERT(def_stmt);
      cout<<def_stmt->unparseToString()<<"  @ line "<<def_stmt->get_file_info()->get_line()
        <<":"<<def_stmt->get_file_info()->get_col() <<endl;
    }
  }
}

/******************************************************************************************************************************/
/*
  Check if the loop is innermostloop.  If yes, it is candidate for SIMD translation.
  Otherwise, it needs more transformation to perform SIMD. 
  We perform the node query on a forStatement.  If the result has only one forStatement from the query,
  this imply it's a innermost loop.
*/
/******************************************************************************************************************************/
bool SIMDAnalysis::isInnermostLoop(SgForStatement* forStatement)
{
  Rose_STL_Container<SgNode*> innerLoopList = NodeQuery::querySubTree (forStatement,V_SgForStatement);
  if(innerLoopList.size() == 1){
    return true;
  }
  else{
    return false;
  }
}

/******************************************************************************************************************************/
/*
  Check if the loop has stride distance 1.  
  We only test this after the loop normalization.  Therefore, the increment expression has only two cases:
  1. i += 1;
  2. i = i +/- k;
*/
/******************************************************************************************************************************/

bool SIMDAnalysis::isStrideOneLoop(SgNode* loop)
{
  // Get the increment expression
  SgExpression* increment = NULL;
  if (SgFortranDo* doLoop = isSgFortranDo(loop))
  {
    increment = doLoop->get_increment();
  }
  else if  (SgForStatement* forLoop = isSgForStatement(loop))
  {
    increment = forLoop->get_increment();
  }
  else
  {
    ROSE_ASSERT(false);
  }

  switch (increment->variantT()) 
  {
    case V_SgAssignOp:
      {
        SgAssignOp* assignOp = isSgAssignOp(increment);
        SgVarRefExp* lhs = isSgVarRefExp(assignOp->get_rhs_operand());
        SgAddOp* addOp = isSgAddOp(assignOp->get_rhs_operand());
        if((lhs != NULL) && (addOp != NULL))
        {
          SgExpression* rhs = addOp->get_rhs_operand();
          SgVarRefExp* leftOperand = isSgVarRefExp(addOp->get_lhs_operand());
          SgIntVal* strideDistance = isSgIntVal(rhs);
          return ((leftOperand != NULL) && 
                  (lhs->get_symbol() == leftOperand->get_symbol()) &&
                  (strideDistance != NULL) && 
                  (strideDistance->get_value() == 1));
        }
        else
        {
          return false;
        }
      }
      break;
    case V_SgPlusAssignOp:
      {
        SgPlusAssignOp* plusAssignOp = isSgPlusAssignOp(increment);
        SgExpression* rhs = plusAssignOp->get_rhs_operand();
        SgIntVal* strideDistance = isSgIntVal(rhs);
        return ((strideDistance != NULL) && (strideDistance->get_value() == 1));
      }
      break;
    default:
      return false;
  }
}
