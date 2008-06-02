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
#include "rose.h"
//#include "RoseBin_support.h"
#include "MyAstAttribute.h"
//#include "RoseBin_unparse_visitor.h"
//#include "../graph/RoseBin_DotGraph.h"
//#include "../graph/RoseBin_GmlGraph.h"

#include <cstdlib>

class RoseBin_FlowAnalysis : public AstSimpleProcessing {
 protected:
   rose_hash::hash_map <uint64_t, SgAsmInstruction* > rememberInstructions; // Insn address -> ROSE insn

  typedef rose_hash::hash_map< uint64_t, SgDirectedGraphNode*> tabletype_inv;

  //tabletype_inv usetable_instr;
  tabletype_inv deftable_instr;


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
  SgDirectedGraphNode* funcDeclNode;

  
  // worklist to build the CFG graph
  std::stack <SgAsmInstruction*> worklist_forthisfunction;
  // visited map for the CFG graph
  rose_hash::hash_map <std::string, SgAsmInstruction*> local_visited;

  typedef std::map<std::string, SgAsmFunctionDeclaration*> bin_funcs_type;
  bin_funcs_type bin_funcs;

  // vector of graphs
  rose_hash::hash_map <std::string, SgDirectedGraph*> graphs;

  static bool initialized;

  VirtualBinCFG::AuxiliaryInformation* info;

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

 public:
  //  RoseBin* roseBin;

  RoseBin_FlowAnalysis(SgAsmNode* global, VirtualBinCFG::AuxiliaryInformation* cfgInfo) {
    info = cfgInfo;
    nrNodes=0;
    nrEdges=0;
    db = RoseBin_support::getDataBaseSupport();
    RoseBin_support::setDebugMode(true);    
    RoseBin_support::setDebugModeMin(true);    
    func_nr=0;
    globalBin = global;
    // todo: optimize later
    if (initialized==false) {
      // (tps 2Jun08) : Jeremiah implemented functions in his disassembler, 
      // so we do not need to perform a conversion from block to function anymore.
      // However, for now we want to pertain the flat hierarchy of function-instruction
      // instead of function-block-instruction and hence have to convert this.
      if (!db)
	flattenBlocks(globalBin);

#if 0
      if (!db)
	convertBlocksToFunctions(globalBin);
#endif
      initFunctionList(globalBin);
      //process_jumps();
#if 0
      if (!db) {
      	resolveFunctions(globalBin);
      }
#endif
      process_jumps();
      initialized = true;
    }
    //    deftable_instr.clear();
    //usetable_instr.clear();
  }
  virtual ~RoseBin_FlowAnalysis() {}

  bool forward_analysis;

  // run this analysis
  virtual void run(RoseBin_Graph* vg, std::string fileN, bool multiedge) =0;

  std::string getName() { return analysisName;}

  void visit(SgNode* node);
  void checkControlFlow( SgAsmInstruction* binInst,
			 int functionSize, int countDown,
			 std::string& currentFunctionName, int func_nr);

  SgDirectedGraphNode* 
    getNodeFor(uint64_t inst) { return deftable_instr[inst];}


  void createInstToNodeTable();
  uint64_t getAddressForNode(SgDirectedGraphNode* node);

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


};

#endif

