#ifndef CFANALYZER_H
#define CFANALYZER_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "SgNodeHelper.h"
#include "Labeler.h"
#include "CommandLineOptions.h"
#include "Flow.h"
#include "FunctionIdMapping.h"
#include "FunctionCallMapping.h"
#include <map>

namespace CodeThorn {

/*!
 *  \brief Constructing the (interprocedural) control-flow graph based on the algorithm presented in [1].
 *
 *  It is fully based on labels for relevant nodes that are determined as the initial step.
 *  The labels are then connected to form the final graph.
 *  The algorithm works as follows:
 *  1) It uses the Labeler to determine which nodes in the program are relevant for the CFG construction
 *  2) For all relevant nodes in the program:
 *    2.1) Determine set of initial labels, e.g., a label for the first initializer in a for loop (see initialLabel(SgNode *node))
 *    2.2) Determine set of final labels, e.g., a label for the condition node of a for loop (see finalLabels(SgNode *node))
 *  3) Based on the determined labels, recursively:
 *    3.1) Construct the graph for a single (AST) node, e.g., for loop (see flow(SgNode *node))
 *         General rule: The labels are used to connect the final labels of the prior node to the inital label of the next node.
 *         Exceptions are when creating fork/join and workshare/barrier nodes in the parallel CFG. Here, the final labels of the enclosed for loop need to be connected to the final label, i.e., the barrier, of the enclosing OpenMP for statement. In a similar fashion need the final labels of the enclosed structured block be connected to the final label of the enclosing OpenMP parallel statement.
 *    3.2) Construct the flow between (AST) nodes, e.g., within a basic block (see flow(SgNode *n1, SgNode *n2))
 *
 * [1] Nielson, F. and Nielson, H. and Hankin, C.: Principles of Program Analysis. 1999, Springer-Verlag Berlin Heidelberg. ISBN: 978-3-642-08474-4.
 *
 * \author Markus Schordan, Jan-Patrick Lehr
 * \date 2012.
 */
class CFAnalysis {
 public:
  CFAnalysis(CodeThorn::Labeler* l);
  CFAnalysis(CodeThorn::Labeler* l, bool createLocalEdge);
  Label getLabel(SgNode* node);
  SgNode* getNode(Label label);
  /**
   * \brief Returns the initial label w.r.t. control flow for node. There is always one.
   */
  Label initialLabel(SgNode* node);
  /**
   * \brief Returns the final labels w.r.t. control flow for a node. There can be multiple.
   *
   * The final label of a node is the node that is visited last in its control flow. Examples are the last statement in a BasicBlock, or the condition in a for loop.
   * For parallel nodes the final label is the join (SgOmpParallelStatement) or the barrier (SgOmpForStatement, SgOmpSectionsStatement) node.
   */
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
  // returns the set of all labels in a function (excluding SgBasicBlock labels)
  LabelSet functionLabelSet(Label entryLabel, Flow& flow);
  LabelSet initialLabelsOfStmtsInBlockSet(SgNode* node);
  LabelSet labelsOfInterestSet();
  // computes a map where the entry label of the corresponding function is provided
  // if the label is not inside a function then the returned label is an invalid label
  InterFlow::LabelToFunctionMap labelToFunctionMap(Flow& flow);
  /**
   * \brief Computes the control flow for an AST subtree rooted at node.
   */
  Flow flow(SgNode* node);
  /**
   * \brief Computes the control flow between two AST subtrees rooted at s1 and s2.
   */
  Flow flow(SgNode* s1, SgNode* s2);
  CodeThorn::Labeler* getLabeler();

  // computes from existing intra-procedural flow graph(s) the inter-procedural call information
  InterFlow interFlow(Flow& flow);
  // experimental
  InterFlow interFlow2(Flow& flow);
  void intraInterFlow(Flow&, InterFlow&);
  // Function for setting a pre-computed function-id
  // mapping. Required for function call resolution across multiple
  // files, and function pointers.
  // deprecated (use FunctionCallMapping instead)
  void setFunctionIdMapping(FunctionIdMapping*);
  FunctionIdMapping* getFunctionIdMapping();
  void setFunctionCallMapping(FunctionCallMapping* fcm);
  void setFunctionCallMapping2(FunctionCallMapping2* fcm);
  FunctionCallMapping* getFunctionCallMapping();
  FunctionCallMapping2* getFunctionCallMapping2();

  Flow controlDependenceGraph(Flow& controlFlow);
  int reduceNode(Flow& flow, Label lab);
  // eliminates only block begin nodes, but not block end nodes.
  int reduceBlockBeginNodes(Flow& flow);
  // eliminates only block end nodes, but not block begin nodes.
  int reduceBlockEndNodes(Flow& flow);
  // eliminates block begin and block end nodes.
  int reduceBlockBeginEndNodes(Flow& flow);

  // reduce to call graph, where each function is represented by its function entry label
  // external functions are not represented (if they are only represented by external edges in the ICFG)
  int reduceToFunctionEntryNodes(Flow& flow);
  /*
    eliminates empty codition nodes. This requires that only one successor is left in the icfg.
    this function can be applied after optimizations (e.g. empty blocks have been removed from the icfg).
  */
  int reduceEmptyConditionNodes(Flow& flow);
  // calls functions reduceBlockBeginEndNodes and reduceEmptyConditionNodes (in this order).
  int optimizeFlow(Flow& flow);

  /**
   * \brief Checks the parallel CFG for consistency w.r.t. fork/join and barriers.
   *
   * Basic checks it performs:
   * 1) There are as many fork labels as join labels in the total set of labels.
   * 2) There are at least as many workshare labels as there are barrier nodes. Their number can be different, as OpenMP's nowait clause can remove barrier nodes.
   */
  bool forkJoinConsistencyChecks(Flow &flow) const;

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
  enum FunctionResolutionMode { FRM_TRANSLATION_UNIT, FRM_WHOLE_AST_LOOKUP, FRM_FUNCTION_ID_MAPPING, FRM_FUNCTION_CALL_MAPPING };
  static FunctionResolutionMode functionResolutionMode;
  static Sawyer::Message::Facility logger;
  void setInterProcedural(bool flag); // by default true
  bool getInterProcedural();
 protected:
  SgFunctionDefinition* determineFunctionDefinition2(SgFunctionCallExp* funCall);
  SgFunctionDefinition* determineFunctionDefinition3(SgFunctionCallExp* funCall);
  FunctionCallTargetSet determineFunctionDefinition4(SgFunctionCallExp* funCall);
  FunctionCallTargetSet determineFunctionDefinition5(Label lbl, SgLocatedNode* astnode);
  static void initDiagnostics();
  void createInterProceduralCallEdges(Flow& flow, InterFlow& interFlow);
  void createIntraProceduralCallEdges(Flow& flow, InterFlow& interFlow);
  bool _interProcedural=true;
 private:
  SgStatement* getCaseOrDefaultBodyStmt(SgNode* node);
  Flow WhileAndDoWhileLoopFlow(SgNode* node, Flow edgeSet, EdgeType param1, EdgeType param2);
  CodeThorn::Labeler* labeler;
  bool _createLocalEdge;
  SgNode* correspondingLoopConstruct(SgNode* node);
  FunctionIdMapping* _functionIdMapping=nullptr;
  FunctionCallMapping* _functionCallMapping=nullptr;
  FunctionCallMapping2* _functionCallMapping2=nullptr;
};

} // end of namespace CodeThorn

#endif
