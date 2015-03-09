#include "sage3basic.h"
#include <iostream>
using namespace std;

#include "CollectionOperators.h"
using namespace CodeThorn;

#include "PropertyState.h"
#include "NumberIntervalLattice.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "IntervalTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

using namespace SPRAY;

IntervalTransferFunctions::IntervalTransferFunctions():
  _cppExprEvaluator(0),
  _domain(0),
  _labeler(0),
  _variableIdMapping(0)
{
}

IntervalTransferFunctions::IntervalTransferFunctions(
                                                     NumberIntervalLattice* domain, 
                                                     PropertyState* p, 
                                                     Labeler* l, 
                                                     VariableIdMapping* vid)
  :_domain(domain),_labeler(l),_variableIdMapping(vid) {
  _cppExprEvaluator=new CppExprEvaluator(domain,p,vid);
}

IntervalTransferFunctions::~IntervalTransferFunctions() {
  if(_cppExprEvaluator)
    delete _cppExprEvaluator;
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& pstate) {
  //ROSE_ASSERT(_variableIdMapping); TODO
  NumberIntervalLattice niLattice;
  CppExprEvaluator evaluator(&niLattice,&pstate,_variableIdMapping);
  cout<<"TINFO: transferExpression "<<node->unparseToString()<<endl;
  evaluator.evaluate(node); // ignore return value for now
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
//NOTE: missing: UD must take uses in initializers into account
void IntervalTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, Lattice& element) {
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
}


/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element) {
  // uses and defs in argument-expressions
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    transferExpression(lab,*i,element);
  }
}
/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, Lattice& element) {
  //TODO: def in x=f(...) (not seen as assignment)
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) {
  // generate Intervals for each parameter variable
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    
    // TODO: element. ...
    // VariableId formalParameterVarId=_variableIdMapping->variableId(formalParameterName);

  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void IntervalTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, Lattice& element) {
  // remove all declared variable at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    // VariableId varId=*i;
    // TODO: element.removeVariableFromState(varId);
  }
  // TODO:: return variable $r
}
