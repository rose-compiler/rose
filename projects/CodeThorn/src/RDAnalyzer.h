#ifndef RDANALYZER_H
#define RDANALYZER_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "AnalysisAbstractionLayer.h"

using namespace std;
using namespace CodeThorn;

#include "RDTransferFunctions.hpp"
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class RDAnalyzer : public DFAnalyzer<RDLattice> {
 public:
  RDAnalyzer();
  ~RDAnalyzer();
  void attachInInfoToAst(string attributeName);
  void attachOutInfoToAst(string attributeName);
  RDLattice transfer(Label lab, RDLattice element);

  typedef AnalyzerData::iterator iterator;
  iterator begin();
  iterator end();
  size_t size();

 private:
  void attachInfoToAst(string attributeName,bool isInInfo);
#if 0
  void transferExpression(Label label, SgExpression* expr, RDLattice& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, RDLattice& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, RDLattice& element);
  void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, RDLattice& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, RDLattice& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, RDLattice& element);
#else
  RDTransferFunctions* _rdTransferFunctions;
#endif
};

#endif
