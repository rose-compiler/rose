#ifndef CFANALYZER_H
#define CFANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "SgNodeHelper.h"
#include "Labeler.h"
#include "CommandLineOptions.h"
#include "Flow.h"

namespace SPRAY {

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class CFAnalyzer {
 public:
  CFAnalyzer(SPRAY::Labeler* l);
  Label getLabel(SgNode* node);
  SgNode* getNode(Label label);
  Label initialLabel(SgNode* node);
  LabelSet finalLabels(SgNode* node);
  LabelSet functionCallLabels(Flow& flow);
  LabelSet functionEntryLabels(Flow& flow);
  LabelSet conditionLabels(Flow& flow);
  Label correspondingFunctionExitLabel(Label entryLabel);
  Label correspondingFunctionCallReturnLabel(Label callLabel);
  int numberOfFunctionParameters(Label entryNode);
  bool isVoidFunction(Label entryNode);
  LabelSetSet functionLabelSetSets(Flow& flow);
  LabelSet functionLabelSet(Label entryLabel, Flow& flow);
  LabelSet setOfInitialLabelsOfStmtsInBlock(SgNode* node);
  Flow flow(SgNode* node);
  Flow flow(SgNode* s1, SgNode* s2);
  SPRAY::Labeler* getLabeler();
  // computes from existing intra-procedural flow graph(s) the inter-procedural call information
  InterFlow interFlow(Flow& flow); 
  void intraInterFlow(Flow&, InterFlow&);
  Flow controlDependenceGraph(Flow& controlFlow);
  int reduceNode(Flow& flow, Label lab);
  int reduceBlockBeginNodes(Flow& flow);
  int reduceEmptyConditionNodes(Flow& flow);

  /*! 
   * This function performs inlining on the ICFG by reducing
   * call/entry/exit/callreturn-nodes, if the function being called is
   * a "trivial" function. A "trivial" function has no formal parameters and
   * is called exactly once in a program.
   */
  int inlineTrivialFunctions(Flow& flow);
  size_t deleteFunctionCallLocalEdges(Flow& flow);
 private:
  Flow WhileAndDoWhileLoopFlow(SgNode* node, Flow edgeSet, EdgeType param1, EdgeType param2);
  SPRAY::Labeler* labeler;
};    

} // end of namespace CodeThorn

#endif
