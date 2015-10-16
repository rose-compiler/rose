#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
#include "LVTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

SPRAY::LVTransferFunctions::LVTransferFunctions() {
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element0) {
  LVLattice& element=dynamic_cast<LVLattice&>(element0);

  // update analysis information
  // this is only correct for RERS12-C programs
  // 1) remove all pairs with lhs-variableid
  // 2) add (lab,lhs.varid)

  // KILL
  // (for programs with pointers we require a set here)
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*getVariableIdMapping());
  ROSE_ASSERT(_pointerAnalysisInterface);

  if(hasDereferenceOperation(node)) {
    VariableIdSet modVarIds=_pointerAnalysisInterface->getModByPointer();
    // union sets
    defVarIds+=modVarIds;
  }
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // since multiple memory locations may be modified, we cannot know which one will be updated and cannot remove information
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.removeVariableId(var);
  }
  // GEN
  VariableIdSet useVarIds=AnalysisAbstractionLayer::useVariables(node,*getVariableIdMapping());  
  for(VariableIdMapping::VariableIdSet::iterator i=useVarIds.begin();i!=useVarIds.end();++i) {
    element.insertVariableId(*i);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, Lattice& element0) {
  LVLattice& element=dynamic_cast<LVLattice&>(element0);

  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  // same as in transferExpression ... needs to be refined
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*getVariableIdMapping());  
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // since multiple memory locations may be modified, we cannot know which one will be updated and cannot remove information add information
    assert(0);
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.removeVariableId(var);
  }

  SgExpression* initExp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(declnode);  
  VariableIdSet useVarIds=AnalysisAbstractionLayer::astSubTreeVariables(initExp,*getVariableIdMapping());  
  for(VariableIdMapping::VariableIdSet::iterator i=useVarIds.begin();i!=useVarIds.end();++i) {
    element.insertVariableId(*i);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionCall(Label lab,  SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element0) {
  LVLattice& element=dynamic_cast<LVLattice&>(element0);

  // uses and defs in argument-expressions
  int paramNr=0;
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    transferExpression(lab,*i,element0);

    // kill parameter variables
    VariableId paramId=getParameterVariableId(paramNr);
    element.removeVariableId(paramId);
    paramNr++;
  }

}
/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionCallReturn(Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element0) {
  LVLattice& element=dynamic_cast<LVLattice&>(element0);

  // kill
  if(lhsVar) {
    VariableId varId=getVariableIdMapping()->variableId(lhsVar);
    element.removeVariableId(varId);
  }
  // gen return variable
  VariableId resVarId=getResultVariableId();
  element.insertVariableId(resVarId);

}
/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element0) {
  LVLattice& element=dynamic_cast<LVLattice&>(element0);

  int paramNr=0;
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    // kill formal parameter
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=getVariableIdMapping()->variableId(formalParameterName);
    element.removeVariableId(formalParameterVarId);

    // generate live function-call passing parameter var
    VariableId paramId=getParameterVariableId(paramNr);
    element.insertVariableId(paramId);
    paramNr++;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, Lattice& element0) {
  // kill return variable
  VariableId resVarId=getResultVariableId();
  element.removeVariableId(resVarId);
}
