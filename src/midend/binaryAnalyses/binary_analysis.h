#include "integerOps.h"
#include "x86InstructionSemantics.h"
#include "Partitioner.h"

/*********************************
* Find root nodes in the graph. A root node is a node with no in edges.
*********************************/
std::vector<SgGraphNode*> findGraphRoots(SgIncidenceDirectedGraph* graph);

/*******************************************************************
 * Generate all static traces of length smaller than a limit.
 *******************************************************************/
void
findTraceForSubtree(const SgIncidenceDirectedGraph* graph, SgGraphNode* cur_node, std::vector<SgAsmx86Instruction*>& curTrace, size_t max_length,
    std::set< std::vector<SgAsmx86Instruction*> >& returnSet);

/*******************************************************************
 * Generate all static traces of length smaller than a limit.
 * We ignore nop's 
 *******************************************************************/
std::set< std::vector<SgAsmx86Instruction*> > 
generateStaticTraces(SgIncidenceDirectedGraph* graph, size_t max_length);

/***************************************************
 * The function addBlocksFromFunctionToGraph creates the SgGraphNode's 
 * for all blocks in the subtree
 **************************************************/
void addBlocksFromFunctionToGraph(SgIncidenceDirectedGraph* graph, std::map<rose_addr_t, SgGraphNode*>& instToNodeMap, SgAsmFunction* funcDecl );

/***************************************************************************************************
 * The goal of the function is to create a control flow graph where nodes are basic blocks.
 */
SgIncidenceDirectedGraph* constructCFG_BB(SgAsmStatement* blocks, const Partitioner::BasicBlockStarts& bb_starts,const Disassembler::InstructionMap& instMap );

/***************************************************************************************************
 * The goal of the function is to create a control flow graph.
 */
SgIncidenceDirectedGraph* constructCFG( SgAsmInterpretation *interp );
SgIncidenceDirectedGraph* constructCFG( const  Partitioner::FunctionStarts& func_starts, const Partitioner::BasicBlockStarts& bb_starts,const Disassembler::InstructionMap& instMap );

/****************************
 * The goal of this function is to create a binary control flow graph. It does this by construcing a CFG and then finding the
 * subset of the CFG that is a call graph.
 */
SgIncidenceDirectedGraph* constructCallGraph( SgAsmInterpretation *interp );
SgIncidenceDirectedGraph* constructCallGraph( const  Partitioner::FunctionStarts& func_starts,const Partitioner::BasicBlockStarts& bb_starts, const Disassembler::InstructionMap& instMap  );

