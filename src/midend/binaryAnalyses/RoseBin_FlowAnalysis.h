/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Jul27 07
 * Decription : Control flow Analysis
 ****************************************************/

#ifndef __RoseBin_FlowAnalysis__
#define __RoseBin_FlowAnalysis__

//#include <mysql.h>
#include <stdio.h>
#include <iostream>
//#include "sage3.h"

//#include "RoseBin_support.h"
#include "MyAstAttribute.h"
//#include "RoseBin_unparse_visitor.h"
//#include "../graph/RoseBin_DotGraph.h"
//#include "../graph/RoseBin_GmlGraph.h"

#include <cstdlib>
#include "GraphAlgorithms.h"

//typedef rose_graph_node_edge_hash_multimap edgeType;


// **************** AS DEFINED BY ANDREAS *****************************************
class FindAsmFunctionsVisitor: public std::binary_function<SgNode*, std::vector<SgAsmFunctionDeclaration *>* , void* >
{
  public:
    void* operator()(first_argument_type node, std::vector<SgAsmFunctionDeclaration*>* insns ) const{
      if (isSgAsmFunctionDeclaration(node)) insns->push_back(isSgAsmFunctionDeclaration(node));
      return NULL;
    }
};

class FindSgFunctionsVisitor: public std::binary_function<SgNode*, std::vector<SgFunctionDeclaration *>* , void* >
{
  public:
    void* operator()(first_argument_type node, std::vector<SgFunctionDeclaration*>* insns ) const{
      if (isSgFunctionDeclaration(node)) insns->push_back(isSgFunctionDeclaration(node));
      return NULL;
    }
};


class FindInstructionsVisitor: public std::binary_function<SgNode*, std::vector<SgAsmInstruction *>* , void* >
{
 public:
  void* operator()(first_argument_type node, std::vector<SgAsmInstruction*>* insns ) const{
    if (isSgAsmInstruction(node)) insns->push_back(isSgAsmInstruction(node));
    return NULL;
  }
};


class FindInstructionsVisitorx86: public std::binary_function<SgNode*, std::vector<SgAsmx86Instruction *>* , void* >
{
 public:
  void* operator()(first_argument_type node, std::vector<SgAsmx86Instruction*>* insns ) const{
    if (isSgAsmx86Instruction(node)) insns->push_back(isSgAsmx86Instruction(node));
    return NULL;
  }
};

class FindAsmStatementsVisitor: public std::binary_function<SgNode*, std::vector<SgAsmStatement *>* , void* >
{
 public:
  void* operator()(first_argument_type node, std::vector<SgAsmStatement*>* insns ) const{
    if (isSgAsmStatement(node)) insns->push_back(isSgAsmStatement(node));
    return NULL;
  }
};

class FindAsmStatementsHeaderVisitor: public std::binary_function<SgNode*, std::vector<SgAsmNode *>* , void* >
{
 public:
  void* operator()(first_argument_type node, std::vector<SgAsmNode*>* insns ) const{
    if (isSgAsmStatement(node)) insns->push_back(isSgAsmStatement(node));
    if (isSgAsmExecutableFileFormat(node)) insns->push_back(isSgAsmExecutableFileFormat(node));
    return NULL;
  }
};

class FindStatementsVisitor: public std::binary_function<SgNode*, std::vector<SgStatement *>* , void* >
{
 public:
  void* operator()(first_argument_type node, std::vector<SgStatement*>* insns ) const{
    if (isSgStatement(node))
      //      if (!isSgStatement(node)->get_file_info()->isCompilerGenerated())
	insns->push_back(isSgStatement(node));
	//}
    return NULL;
  }
};

class FindNodeVisitor: public std::binary_function<SgNode*, std::vector<SgLocatedNode *>* , void* >
{
 public:
  void* operator()(first_argument_type node, std::vector<SgLocatedNode*>* insns ) const{
    if (isSgNode(node))
      insns->push_back(isSgLocatedNode(node));
    return NULL;
  }
};

// ************************************************************************************

class RoseBin_FlowAnalysis : public AstSimpleProcessing {//, public GraphAlgorithms {
 public:
  //remove later!
   //typedef rose_hash::unordered_map <std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string> nodeType;


 protected:
   rose_hash::unordered_map <uint64_t, SgAsmInstruction* > rememberInstructions; // Insn address -> ROSE insn

  typedef rose_hash::unordered_map< uint64_t, SgGraphNode*> tabletype_inv;

  //tabletype_inv usetable_instr;
  tabletype_inv deftable_instr;

  int nrOfFunctions;

  //typedef SB_DirectedGraph::nodeType nodeType;
  //typedef rose_hash::unordered_map < std::string, SgDirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string> edgeType;

  SgAsmNode* globalBin;
  int func_nr;
  int nr_target_missed;
  RoseBin_Graph* vizzGraph;
  std::string fileName;
  bool printEdges;
  // the name of the analysis
  std::string analysisName;

  // the string types of nodes and edges
  std::string typeNode;
  std::string typeEdge;

  // needed for CallGraphAnalysis
  SgAsmFunctionDeclaration* funcDecl;
  SgGraphNode* funcDeclNode;



  // worklist to build the CFG graph
  std::stack <SgAsmInstruction*> worklist_forthisfunction;

  // visited map for the CFG graph
// DQ (4/23/2009): We need to specify the default template parameters explicitly.
// rose_hash::unordered_map <std::string, SgAsmInstruction*> local_visited;
// CH (4/9/2010): Use boost::unordered instead
//#ifdef _MSC_VER
#if 0
//  rose_hash::unordered_map <std::string, SgAsmInstruction*,rose_hash::hash_string> local_visited;
  rose_hash::unordered_map <std::string, SgAsmInstruction*> local_visited;
#else
  // CH (4/13/2010): Use boost::hash<string> instead
  //rose_hash::unordered_map <std::string, SgAsmInstruction*,rose_hash::hash_string,rose_hash::eqstr_string> local_visited;
  rose_hash::unordered_map <std::string, SgAsmInstruction*> local_visited;
#endif

  typedef std::map<std::string, SgAsmFunctionDeclaration*> bin_funcs_type;
  bin_funcs_type bin_funcs;

  // vector of graphs
  rose_hash::unordered_map <std::string, SgIncidenceDirectedGraph*> graphs;

  static bool initialized;

  //VirtualBinCFG::AuxiliaryInformation* info;

  void initFunctionList(SgAsmNode* global);
  void process_jumps();
  SgAsmInstruction* process_jumps_get_target(SgAsmx86Instruction* inst);
  void resolveFunctions(SgAsmNode* global);
  SgAsmInstruction* resolveFunction(SgAsmInstruction* inst, bool hasStopCondition);
  void convertBlocksToFunctions(SgAsmNode* globalNode);
  void flattenBlocks(SgAsmNode* globalNode);

  bool db;

  int nrNodes;
  int nrEdges;

  GraphAlgorithms* g_algo;
 public:
  //  RoseBin* roseBin;

// DQ (10/20/2010): Moved to source file to support compilation of language only mode which excludes binary analysis support.
  RoseBin_FlowAnalysis(SgAsmNode* global, GraphAlgorithms* algo);

  virtual ~RoseBin_FlowAnalysis() {}

  void setInitializedFalse() {
    initialized=false;
  }
  RoseBin_Graph* getGraph() {return vizzGraph;}

  bool forward_analysis;
  void printAST(SgAsmNode* globalNode);
  // run this analysis
  virtual void run(RoseBin_Graph* vg, std::string fileN, bool multiedge) =0;

  std::string getName() { return analysisName;}

  void visit(SgNode* node);
  void checkControlFlow( SgAsmInstruction* binInst,
                         int functionSize, int countDown,
			 std::string& currentFunctionName, int func_nr);

  SgGraphNode*
    getNodeFor(uint64_t inst) { return deftable_instr[inst];}


  void createInstToNodeTable();
  uint64_t getAddressForNode(SgGraphNode* node);

  // converts string to hex
  template <class T>
    bool from_string(T& t,
                     const std::string& s,
		     std::ios_base& (*f)(std::ios_base&))
    {
      std::istringstream iss(s);
      return !(iss >> f >> t).fail();
    }

  int nodesVisited() {
    return nrNodes;
  }

  int edgesVisited() {
    return nrEdges;
  }

  bool sameParents(SgGraphNode* node, SgGraphNode* next);
  void getRootNodes(std::vector <SgGraphNode*>& rootNodes);


  SgGraphNode* addCFNode(std::string& name, std::string& type, int address, bool isFunction, SgNode* int_node);

  void clearMaps();

};

#endif

