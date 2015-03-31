#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
using namespace CodeThorn;

#include "LVTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

SPRAY::LVTransferFunctions::LVTransferFunctions() {
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element0) {
  LVLattice* element1=dynamic_cast<LVLattice*>(&element0);
  ROSE_ASSERT(element1);
  LVLattice& element=*element1;
  // update analysis information
  // this is only correct for RERS12-C programs
  // 1) remove all pairs with lhs-variableid
  // 2) add (lab,lhs.varid)

  // KILL
  // (for programs with pointers we require a set here)
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*_variableIdMapping);
  ROSE_ASSERT(_pointerAnalysisInterface);
  VariableIdSet modVarIds=_pointerAnalysisInterface->getModByPointer();
  // union sets
  defVarIds+=modVarIds;
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // since multiple memory locations may be modified, we cannot know which one will be updated and cannot remove information
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.removeVariableId(var);
  }
  // GEN
  VariableIdSet useVarIds=AnalysisAbstractionLayer::useVariables(node,*_variableIdMapping);  
  for(VariableIdMapping::VariableIdSet::iterator i=useVarIds.begin();i!=useVarIds.end();++i) {
    element.insertVariableId(*i);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, Lattice& element0) {
  LVLattice* element1=dynamic_cast<LVLattice*>(&element0);
  ROSE_ASSERT(element1);
  LVLattice& element=*element1;

  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  // same as in transferExpression ... needs to be refined
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*_variableIdMapping);  
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // since multiple memory locations may be modified, we cannot know which one will be updated and cannot remove information add information
    assert(0);
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.removeVariableId(var);
  }
  VariableIdSet useVarIds=AnalysisAbstractionLayer::useVariables(node,*_variableIdMapping);  
  for(VariableIdMapping::VariableIdSet::iterator i=useVarIds.begin();i!=useVarIds.end();++i) {
    element.insertVariableId(*i);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element0) {
  LVLattice* element1=dynamic_cast<LVLattice*>(&element0);
  ROSE_ASSERT(element1);
  LVLattice& element=*element1;

  // uses and defs in argument-expressions
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    transferExpression(lab,*i,element);
  }
}
/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, Lattice& element0) {
  //TODO: def in x=f(...) (not seen as assignment)
}
/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element0) {
  LVLattice* element1=dynamic_cast<LVLattice*>(&element0);
  ROSE_ASSERT(element1);
  LVLattice& element=*element1;

  // remove LVs for each parameter variable
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=_variableIdMapping->variableId(formalParameterName);
    element.removeVariableId(formalParameterVarId);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::LVTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, Lattice& element0) {
  // nothing to do
}
