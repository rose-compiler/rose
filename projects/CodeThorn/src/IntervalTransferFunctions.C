#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
using namespace CodeThorn;

#include "IntervalTransferFunctions.hpp"
#include "AnalysisAbstractionLayer.h"

IntervalTransferFunctions::IntervalTransferFunctions() {
  cppExprEvaluator=0;
}

IntervalTransferFunctions::IntervalTransferFunctions(Domain* domain, PropertyState* p, Labeler* l, VariableIdMapping vid):_domain(domain),_labeler(l),_variableIdMapping(vid){
  cppExprEvaluator=new CppExprEvaluator(domain,p,vid);
}

IntervalTransferFunctions::~IntervalTransferFunctions() {
  if(cppExprEvaluator)
    delete cppExprEvaluator;
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferExpression(Label lab, SgExpression* node, IntervalLattice& element) {

  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
//NOTE: missing: UD must take uses in initializers into account
void IntervalTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, IntervalLattice& element) {
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,IntervalLattice& element) {
  // uses and defs in argument-expressions
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    transferExpression(lab,*i,element);
  }
}
/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, IntervalLattice& element) {
  //TODO: def in x=f(...) (not seen as assignment)
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, IntervalLattice& element) {
  // generate Intervals for each parameter variable
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=_variableIdMapping->variableId(formalParameterName);
    // TODO: element. ...

  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, IntervalLattice& element) {
  // remove all declared variable at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    VariableId varId=*i;
    // TODO: element.removeVariableFromState(varId);
  }
  // TODO:: return variable $r
}
