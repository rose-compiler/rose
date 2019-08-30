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
#include "FunctionIdMapping.h"

namespace CodeThorn {

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class CFAnalysis {
 public:
  CFAnalysis(CodeThorn::Labeler* l);
  CFAnalysis(CodeThorn::Labeler* l, bool createLocalEdge);
  Label getLabel(SgNode* node);
  SgNode* getNode(Label label);
  Label initialLabel(SgNode* node);
  LabelSet finalLabels(SgNode* node);
  LabelSet functionCallLabels(Flow& flow);
  LabelSet functionEntryLabels(Flow& flow);
  LabelSet conditionLabels(Flow& flow);
  // computes function exit label for a provided function entry label
  Label correspondingFunctionExitLabel(Label entryLabel);
  // computes function return label for a provided function call label
  Label correspondingFunctionCallReturnLabel(Label callLabel);
  int numberOfFunctionParameters(Label entryNode);
  bool isVoidFunction(Label entryNode);
  LabelSetSet functionLabelSetSets(Flow& flow);
  LabelSet functionLabelSet(Label entryLabel, Flow& flow);
  LabelSet setOfInitialLabelsOfStmtsInBlock(SgNode* node);
  Flow flow(SgNode* node);
  Flow flow(SgNode* s1, SgNode* s2);
  CodeThorn::Labeler* getLabeler();

  // determine mapping between function calls and function definitions (also resolves function pointers)
  void computeFunctionCallMapping(SgProject* project);
  // computes from existing intra-procedural flow graph(s) the inter-procedural call information
  InterFlow interFlow(Flow& flow); 
  void intraInterFlow(Flow&, InterFlow&);
  // Function for setting a pre-computed function-id
  // mapping. Required for function call resolution across multiple
  // files, and function pointers.
  void setFunctionIdMapping(FunctionIdMapping*);
  FunctionIdMapping* getFunctionIdMapping();

  Flow controlDependenceGraph(Flow& controlFlow);
  int reduceNode(Flow& flow, Label lab);
  // eliminates only block begin nodes, but not block end nodes.
  int reduceBlockBeginNodes(Flow& flow);
  // eliminates only block end nodes, but not block begin nodes.
  int reduceBlockEndNodes(Flow& flow);
  // eliminates block begin and block end nodes.
  int reduceBlockBeginEndNodes(Flow& flow);
  /*
    eliminates empty codition nodes. This requires that only one successor is left in the icfg.
    this function can be applied after optimizations (e.g. empty blocks have been removed from the icfg).
  */
  int reduceEmptyConditionNodes(Flow& flow);
  // calls functions reduceBlockBeginEndNodes and reduceEmptyConditionNodes (in this order).
  int optimizeFlow(Flow& flow);

  /*! 
   * This function performs inlining on the ICFG by reducing
   * call/entry/exit/callreturn-nodes, if the function being called is
   * a "trivial" function. A "trivial" function has no formal parameters and
   * is called exactly once in a program.
   */
  int inlineTrivialFunctions(Flow& flow);
  size_t deleteFunctionCallLocalEdges(Flow& flow);
  static SgStatement* getFirstStmtInBlock(SgBasicBlock* block);
  static SgStatement* getLastStmtInBlock(SgBasicBlock* block);
  void setCreateLocalEdge(bool le);
  bool getCreateLocalEdge();
  static bool isLoopConstructRootNode(SgNode* node);
  enum FunctionResolutionMode { FRM_TRANSLATION_UNIT, FRM_WHOLE_AST_LOOKUP, FRM_FUNCTION_ID_MAPPING };
  static FunctionResolutionMode functionResolutionMode;
 protected:
  SgFunctionDefinition* determineFunctionDefinition2(SgFunctionCallExp* funCall);
  SgFunctionDefinition* determineFunctionDefinition3(SgFunctionCallExp* funCall);
  static void initDiagnostics();
  static Sawyer::Message::Facility logger;
 private:
  SgStatement* getCaseOrDefaultBodyStmt(SgNode* node);
  Flow WhileAndDoWhileLoopFlow(SgNode* node, Flow edgeSet, EdgeType param1, EdgeType param2);
  CodeThorn::Labeler* labeler;
  bool _createLocalEdge;
  SgNode* correspondingLoopConstruct(SgNode* node);
  FunctionIdMapping* _functionIdMapping=nullptr;

};    

} // end of namespace CodeThorn

#endif
