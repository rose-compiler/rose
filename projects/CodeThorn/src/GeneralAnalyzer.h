#ifndef GENERALANALYZER_H
#define GENERALANALYZER_H

// Author: Markus Schordan, 2014

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"
#include "GeneralAnalyzerBase.h"

using namespace std;
using namespace CodeThorn;

/*! 
  * \author Markus Schordan
  * \date 2014.
 */

class GeneralAnalyzer : public GeneralAnalyzerBase {
 public:
  GeneralAnalyzer();
  PropertyState* transfer(Label lab, PropertyState* element);

 protected:
  virtual PropertyState* transferExpression(Label label, SgExpression* expr, PropertyState* element);
  virtual PropertyState* transferDeclaration(Label label, SgVariableDeclaration* decl, PropertyState* element);
  virtual PropertyState* transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, PropertyState* element);
  virtual PropertyState* transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, PropertyState* element);
  virtual PropertyState* transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, PropertyState* element);
  virtual PropertyState* transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, PropertyState* element);
  // for path sensitive analysis (allows to pass different information to successors in ICFG)
  virtual PropertyState* transferCondition(Label sourceLabel, EdgeType edgeType, Label targetLabel, SgExpression* expr, PropertyState* element);
};

#endif
