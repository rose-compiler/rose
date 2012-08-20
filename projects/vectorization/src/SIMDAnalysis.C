#include "SIMDAnalysis.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace SIMDAnalysis;

/******************************************************************************************************************************/
/*
  Get the Def information.

  TODO: This is reserved for later usage.
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

  TODO: This is reserved for later usage.
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
bool SIMDAnalysis::isInnermostLoop(SgNode* n)
{
  Rose_STL_Container<SgNode*> innerLoopList;
  bool result = false;
  switch(n->variantT())
  {
    case V_SgForStatement:
      {
        SgForStatement* forStatement = isSgForStatement(n);
        innerLoopList = NodeQuery::querySubTree (forStatement,V_SgForStatement);
        if(innerLoopList.size() == 1)
          result = true;
        else
           result = false;
      }
      break;
    case V_SgFortranDo:
      {
        SgFortranDo* fortranDo = isSgFortranDo(n);
        innerLoopList = NodeQuery::querySubTree (fortranDo,V_SgFortranDo);
        if(innerLoopList.size() == 1)
          result = true;
        else
          result = false;
      }
      break;
    case V_SgWhileStmt:
      {
        SgWhileStmt* whileStatement = isSgWhileStmt(n);
        innerLoopList = NodeQuery::querySubTree (whileStatement,V_SgWhileStmt);
        if(innerLoopList.size() == 1)
          result = true;
        else
          result = false;
      }
      break;
    default:
      {
        cerr<<"warning, unhandled node type: "<< n->class_name()<<endl;
        result = false;
      }
  }
  return result;
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
  SgInitializedName* ivar = NULL;
  SgExpression* lb = NULL;
  SgExpression* ub = NULL;
  SgExpression* step = NULL;
  SgStatement* orig_body = NULL;
  bool is_canonical = false;

  if (SgFortranDo* doLoop = isSgFortranDo(loop))
  {
    is_canonical = isCanonicalDoLoop(doLoop, &ivar, &lb, &ub, &step, &orig_body, NULL, NULL);
  }
  else if  (SgForStatement* forLoop = isSgForStatement(loop))
  {
    is_canonical = isCanonicalForLoop(forLoop, &ivar, &lb, &ub, &step, &orig_body, NULL, NULL);
  }
  else
  {
    cerr<<"warning, input is not loop. "<< endl;    
    ROSE_ASSERT(false);
  }

  SgIntVal* strideDistance = isSgIntVal(step);
  return (is_canonical && (strideDistance != NULL) && (strideDistance->get_value() == 1));
}
