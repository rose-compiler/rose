#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
#include "RDTransferFunctions.h"
#include "AstUtility.h"

using namespace CodeThorn;

#if 0
bool hasDereferenceOperation(SgExpression* exp) {
  RoseAst ast(exp);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgExpression* exp=isSgExpression(*i)) {
      if(isSgArrowExp(exp)
         ||isSgPointerDerefExp(exp)
         ||(isSgVarRefExp(exp)&&isSgReferenceType(exp->get_type()))
         ) {
        return true;
      }
    }
  }
  return false;
}
#endif

RDTransferFunctions::RDTransferFunctions() {
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */

SgNode* determineDef(SgNode* node, bool lhs) {
  VariantT nodeType=node->variantT();
  if(isSgCompoundAssignOp(node)||isSgAssignOp(node)) {
    return determineDef(SgNodeHelper::getLhs(node),true);
  }
  switch(nodeType) {
  case V_SgVarRefExp: {
    if(lhs) {
      return node;
    } else {
      return 0;
    }
  }
  case V_SgArrowExp:
  case V_SgPointerDerefExp:
    cout<<"WARNING RD Analysis: unsupported pointer op "<<node->unparseToString()<<endl;
    return 0;
  default:
    if(isSgBinaryOp(node)) {
      SgNode* lhsNode=determineDef(SgNodeHelper::getLhs(node),lhs);
      if(lhsNode) {
        return lhsNode;
      } else {
        return determineDef(SgNodeHelper::getRhs(node),lhs);
      }
    }
    if(isSgUnaryOp(node)) {
      return determineDef(SgNodeHelper::getUnaryOpChild(node),lhs);
    }
  }
  return 0;
}

SgNode* determineDef(SgNode* node) {
  return determineDef(node,false);
}

void RDTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element0) {
  // throws bad_cast exception when downcasting to the wrong type
  RDLattice& element=dynamic_cast<RDLattice&>(element0);

  // update analysis information
  // 1) remove all pairs with lhs-variableid
  // 2) add (lab,lhs.varid)
  if(SgNode* lhsNode=determineDef(node)) {
    if(SgVarRefExp* lhsVar=isSgVarRefExp(lhsNode)) {
      VariableId var=getVariableIdMapping()->variableId(lhsVar);
      element.removeAllPairsWithVariableId(var);
      element.insertPair(lab,var);
    }
  } else {
    // no assignments (e.g. condition), nothing to do.
  }

  // (for programs with pointers)
  ROSE_ASSERT(_pointerAnalysisInterface);
  //VariableIdSet defVarIds=AstUtility::defVariables(node,*getVariableIdMapping(), _pointerAnalysisInterface);
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */
//NOTE: missing: UD must take uses in initializers into account
void RDTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  VariableId var=getVariableIdMapping()->variableId(node);
  element.insertPair(lab,var);
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);

  // uses and defs in argument-expressions
  int paramNr=0;
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    VariableId paramId=getParameterVariableId(paramNr);
    transferExpression(lab,*i,element);
    // insert parameter variable
    element.insertPair(lab,paramId);
    paramNr++;
  }
}

void RDTransferFunctions::transferFunctionCallReturn(Label lab, VariableId lhsVarId, SgFunctionCallExp* callExp, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);
  if(lhsVarId.isValid()) {
    element.insertPair(lab,lhsVarId);
  } else {
    // void function call, nothing to do.
  }
  // determine variable-id of dedicated variable for holding the return value
  VariableId resVarId=getResultVariableId();
  // remove variable-id pairs of dedicated variable for holding the return value
  element.removeAllPairsWithVariableId(resVarId);
}

/*!
  * \author Markus Schordan
  * \date 2013, 2015.
 */
void RDTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);

  // generate RDs for each parameter variable
  int paramNr=0;
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {

    // kill parameter variables
    VariableId paramId=getParameterVariableId(paramNr);
    element.removeAllPairsWithVariableId(paramId);

    // generate formal parameter
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=getVariableIdMapping()->variableId(formalParameterName);
    element.insertPair(lab,formalParameterVarId);

    paramNr++;
  }
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);

  // remove all declared variable at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    VariableId varId=*i;
    element.removeAllPairsWithVariableId(varId);
  }
}

void RDTransferFunctions::transferReturnStmtExpr(Label lab, SgExpression* node, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);
  transferExpression(lab,node,element);
  VariableId resVarId=getResultVariableId();
  element.insertPair(lab,resVarId);
}

void RDTransferFunctions::initializeExtremalValue(Lattice& element) {
  RDLattice* rdElement=dynamic_cast<RDLattice*>(&element);
  rdElement->setEmptySet();
  cout<<"INFO: initialized extremal value."<<endl;
}
