#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
#include "RDTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

using namespace SPRAY;

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
  // throws bad_cast exception when downcasting to the wrong type
  RDLattice& element=dynamic_cast<RDLattice&>(element0);

  // update analysis information
  // this is only correct for RERS12-C programs
  // 1) remove all pairs with lhs-variableid
  // 2) add (lab,lhs.varid)

  // (for programs with pointers we require a set here)
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*getVariableIdMapping());  
  ROSE_ASSERT(_pointerAnalysisInterface);
  if(hasDereferenceOperation(node)) {
    VariableIdSet modVarIds=_pointerAnalysisInterface->getModByPointer();
    // union sets
    defVarIds+=modVarIds;
  }
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // If an unknown array element is referenced, we consider
    // all its elements modified in the same statement.
    // Here *var* is an array element if its symbol is equal to at
    // least one of those corresponding to VariableIds with next or previous ids.
    for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
      VariableId var = *i;
      VariableId prev_var_id;
      VariableId next_var_id;
      prev_var_id.setIdCode(var.getIdCode() - 1);
      next_var_id.setIdCode(var.getIdCode() + 1);
      SgSymbol *var_smb = getVariableIdMapping()->getSymbol(var);
      SgSymbol *prev_var_smb = getVariableIdMapping()->getSymbol(prev_var_id);
      SgSymbol *next_var_smb = getVariableIdMapping()->getSymbol(next_var_id);
      if((var_smb == prev_var_smb) || (var_smb == next_var_smb))
        element.removeAllPairsWithVariableId(var);
    }
    // since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
    for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
      element.insertPair(lab,*i);
    }
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.removeAllPairsWithVariableId(var);
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
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  // same as in transferExpression ... needs to be refined
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,*getVariableIdMapping());  
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
    // If an array is defined, we add all its elements to def set.
    // Remove pairs corresponding to array elements as in transferExpression()
    // but now assert that only elements of one array were modified.
    unsigned elements = 0;
    SgSymbol *array_smb = getVariableIdMapping()->getSymbol(*(defVarIds.begin()));
    for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
      VariableId var = *i;
      VariableId prev_var_id;
      VariableId next_var_id;
      prev_var_id.setIdCode(var.getIdCode() - 1);
      next_var_id.setIdCode(var.getIdCode() + 1);
      SgSymbol *var_smb = getVariableIdMapping()->getSymbol(var);
      SgSymbol *prev_var_smb = getVariableIdMapping()->getSymbol(prev_var_id);
      SgSymbol *next_var_smb = getVariableIdMapping()->getSymbol(next_var_id);
      if((var_smb == prev_var_smb) || (var_smb == next_var_smb)) {
        element.removeAllPairsWithVariableId(var);
        elements++;
      }
      if(var_smb != array_smb)
        assert(0); // more than one array is modified
    }
    // since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
    for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
      element.insertPair(lab,*i);
    }
    if(elements != defVarIds.size())
      assert(0);
  } else if(defVarIds.size()==1) {
    // one unique memory location (variable). We can remove all pairs with this variable
    VariableId var=*defVarIds.begin();
    element.removeAllPairsWithVariableId(var);
    element.insertPair(lab,var);
  }
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
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionCallReturn(Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);
  if(lhsVar) {
    VariableId varId=getVariableIdMapping()->variableId(lhsVar);
    element.insertPair(lab,varId);
  }
  // determine variable-id of dedicated variable for holding the return value
  VariableId resVarId=getResultVariableId();
  // remove variable-id pairs of dedicated variable for holding the return value
  element.removeAllPairsWithVariableId(resVarId);
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element0) {
  RDLattice& element=dynamic_cast<RDLattice&>(element0);

  // generate RDs for each parameter variable
  int paramNr=0;
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=getVariableIdMapping()->variableId(formalParameterName);
    element.insertPair(lab,formalParameterVarId);
    VariableId paramId=getParameterVariableId(paramNr);
    element.removeAllPairsWithVariableId(paramId);
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
