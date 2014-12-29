#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
using namespace CodeThorn;

#include "RDTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

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

RDTransferFunctions::RDTransferFunctions() {
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element0) {
  RDLattice* element1=dynamic_cast<RDLattice*>(&element0);
  ROSE_ASSERT(element1);
  RDLattice& element=*element1;
  // update analysis information
  // this is only correct for RERS12-C programs
  // 1) remove all pairs with lhs-variableid
  // 2) add (lab,lhs.varid)

  // (for programs with pointers we require a set here)
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*_variableIdMapping);  
  ROSE_ASSERT(_pointerAnalysisInterface);
  if(hasDereferenceOperation(node)) {
    VariableIdSet modVarIds=_pointerAnalysisInterface->getModByPointer();
    // union sets
    defVarIds+=modVarIds;
  }
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
    for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
      element.insertPair(lab,*i);
    }
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.eraseAllPairsWithVariableId(var);
    element.insertPair(lab,var);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
//NOTE: missing: UD must take uses in initializers into account
void RDTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);
  //  ROSE_ASSERT(element1);
  //RDLattice& element=*element1;

  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  // same as in transferExpression ... needs to be refined
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*_variableIdMapping);  
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
    for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
      element.insertPair(lab,*i);
    }
    assert(0);
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.eraseAllPairsWithVariableId(var);
    element.insertPair(lab,var);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element0) {
  RDLattice* element1=dynamic_cast<RDLattice*>(&element0);
  ROSE_ASSERT(element1);
  RDLattice& element=*element1;

  // uses and defs in argument-expressions
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    transferExpression(lab,*i,element);
  }
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, Lattice& element0) {
  //TODO: def in x=f(...) (not seen as assignment)
}
//NOTE: UD analysis must take uses of function-call arguments into account
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element0) {
  RDLattice* element1=dynamic_cast<RDLattice*>(&element0);
  ROSE_ASSERT(element1);
  RDLattice& element=*element1;

  // generate RDs for each parameter variable
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=_variableIdMapping->variableId(formalParameterName);
    // it must hold that this VarId does not exist in the RD-element
    //assert
    element.insertPair(lab,formalParameterVarId);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, Lattice& element0) {

  RDLattice* element1=dynamic_cast<RDLattice*>(&element0);
  ROSE_ASSERT(element1);
  RDLattice& element=*element1;

  // remove all declared variable at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    VariableId varId=*i;
    element.eraseAllPairsWithVariableId(varId);
  }
  // TODO:: return variable $r
}
