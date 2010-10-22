// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "stringify.h"

#include "AsmUnparser_compat.h"
#include <binary_analysis.h>


#include <boost/lexical_cast.hpp>

/*********************************
* Find root nodes in the graph. A root node is a node with no in edges.
*********************************/
std::vector<SgGraphNode*> findGraphRoots(SgIncidenceDirectedGraph* graph)
{
  std::vector<SgGraphNode*> roots;

  rose_graph_integer_node_hash_map & nodes =
    graph->get_node_index_to_node_map ();

  
  rose_graph_integer_edge_hash_multimap & inEdges
        = graph->get_node_index_to_edge_multimap_edgesIn ();

  for( rose_graph_integer_node_hash_map::iterator it = nodes.begin();
      it != nodes.end(); ++it )
  {
    SgGraphNode* node = it->second;

    if( inEdges.find(node->get_index()) == inEdges.end() )
      roots.push_back(node);
  };

  return roots;
  
};


/*******************************************************************
 * Generate all static traces of length smaller than a limit.
 *******************************************************************/
void
findTraceForSubtree(const SgIncidenceDirectedGraph* graph, SgGraphNode* cur_node, std::vector<SgAsmx86Instruction*>& curTrace, size_t max_length,
    std::set< std::vector<SgAsmx86Instruction*> >& returnSet)
{


   const rose_graph_integer_edge_hash_multimap&  outEdges
             = graph->get_node_index_to_edge_multimap_edgesOut ();


   std::pair< rose_graph_integer_edge_hash_multimap::const_iterator,
      rose_graph_integer_edge_hash_multimap::const_iterator> outEdgeIt
     = outEdges.equal_range( cur_node->get_index() );



   //Add all instructions from the current block to the current trace
   ROSE_ASSERT(isSgAsmBlock(cur_node->get_SgNode() ) );

   for(SgAsmStatementPtrList::iterator blockItr =  isSgAsmBlock(cur_node->get_SgNode() )->get_statementList ().begin();
       blockItr != isSgAsmBlock(cur_node->get_SgNode() )->get_statementList ().end() ; ++blockItr )
   {
     ROSE_ASSERT(isSgAsmx86Instruction(*blockItr ) );

     //Ignore nop's
     if( isSgAsmx86Instruction(*blockItr)->get_kind() != x86_nop )
       curTrace.push_back( isSgAsmx86Instruction(*blockItr ) );
   }


   //find next node
   //  create edge
   //  if branch create copy vector and crete new list for false condition 

   if( outEdgeIt.first != outEdgeIt.second && curTrace.size() < max_length )
   {
     SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt.first->second);
     findTraceForSubtree(graph, graphEdge->get_to(), curTrace, max_length, returnSet );
     for( ; outEdgeIt.first != outEdgeIt.second; ++outEdgeIt.first )
     {
       SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt.first->second);
       ROSE_ASSERT(graphEdge!=NULL);

       //Branch and create new trace for branch
       std::vector<SgAsmx86Instruction*> newTrace = curTrace;

       findTraceForSubtree(graph, graphEdge->get_to(), newTrace, max_length, returnSet);
     }

   }else if ( curTrace.size() > 0 )
     returnSet.insert(curTrace);



};


/*******************************************************************
 * Generate all static traces of length smaller than a limit.
 * We ignore nop's 
 *******************************************************************/
std::set< std::vector<SgAsmx86Instruction*> > 
generateStaticTraces(SgIncidenceDirectedGraph* graph, size_t max_length)
{
  std::vector<SgGraphNode*> roots = findGraphRoots(graph);

  std::set< std::vector<SgAsmx86Instruction*> > returnSet;

  for( std::vector<SgGraphNode*>::iterator rootItr = roots.begin(); rootItr != roots.end(); ++rootItr )
  {
    //find next node
    //  create edge
    //  if branch create copy vector and crete new list for false condition 


    std::vector<SgAsmx86Instruction*> curTrace;
    std::set< std::vector<SgAsmx86Instruction*> > currentSet;
    findTraceForSubtree(graph, *rootItr, curTrace, max_length, currentSet);


    for( std::set< std::vector<SgAsmx86Instruction*> >::iterator curSetItr = currentSet.begin(); curSetItr != currentSet.end(); ++curSetItr  )
      returnSet.insert(*curSetItr);


  }

  return returnSet;

};


/***************************************************
 * The function addBlocksFromFunctionToGraph creates the SgGraphNode's 
 * for all blocks in the subtree
 **************************************************/
void addBlocksFromFunctionToGraph(SgIncidenceDirectedGraph* graph, std::map<rose_addr_t, SgGraphNode*>& instToNodeMap, SgAsmFunctionDeclaration* funcDecl )
{
  ROSE_ASSERT(funcDecl!=NULL);

  //Need to create a complete list of basic blocks as some basic blocks without incoming edges do not show up 
  //in bb_starts


  std::vector<SgNode*> blockList = NodeQuery::querySubTree(funcDecl, V_SgAsmBlock);

  SgAsmBlock* prevBB = NULL;

  SgGraphNode* prevNode = NULL;

  for( std::vector<SgNode*>::iterator blockItr = blockList.begin(); blockItr != blockList.end(); ++blockItr )
  {
    SgAsmBlock* block = isSgAsmBlock(*blockItr);

    ROSE_ASSERT( block != NULL );
    //    SgGraphNode* node = new SgGraphNode( boost::lexical_cast<std::string> ( block->get_address() ) );

    std::string label;

    std::string opCode;


    if(block->get_statementList().size()>0 )
    {
#ifndef _MSC_VER
      opCode = stringifyX86InstructionKind(isSgAsmx86Instruction( block->get_statementList().back() )->get_kind());
#else
	opCode = "";
	ROSE_ASSERT(false);
#endif

	  std::cout <<"Statement opcode " << opCode << std::endl;
    }else
      std::cout <<"No statements " << block->class_name() << std::endl;

    label=  boost::lexical_cast<std::string> ( block->get_address() ) +" "+  unparseInstruction( isSgAsmx86Instruction(block->get_statementList().back()));
    SgGraphNode* node = new SgGraphNode( boost::lexical_cast<std::string> ( label ) );

    node->set_SgNode(block);

    graph->addNode(node);
    instToNodeMap[ block->get_address() ] = node;

#if 0
    if( prevBB != NULL )
      if( prevBB->get_statementList().size()>0 && isSgAsmx86Instruction(prevBB->get_statementList().back())->get_kind() == x86_call )
      {

        //        returnGraph->addDirectedEdge( prevNode, node  , " ");

      }
#endif

    prevBB = block;
    prevNode = node;

  }
}



/***************************************************************************************************
 * The goal of the function is to create a control flow graph.
 */
SgIncidenceDirectedGraph* constructCFG_BB(SgAsmStatement* blocks, const Partitioner::BasicBlockStarts& bb_starts,const Disassembler::InstructionMap& instMap )
{
  SgIncidenceDirectedGraph *returnGraph = new SgIncidenceDirectedGraph();
  ROSE_ASSERT (returnGraph != NULL);

  ROSE_ASSERT( isSgAsmBlock(blocks) || isSgAsmFunctionDeclaration(blocks)  );

  typedef std::map<rose_addr_t, SgGraphNode*> InstructionToNode;

  InstructionToNode instToNodeMap;

  //Add all nodes to the graph


  //Iterate over all functions to add nodes to the graph

  if( isSgAsmFunctionDeclaration(blocks) != NULL ) // For Single  function
  {
    SgAsmFunctionDeclaration* funcDecl = isSgAsmFunctionDeclaration(blocks);
    addBlocksFromFunctionToGraph(returnGraph, instToNodeMap, funcDecl);
  }else
    for( unsigned int i = 0 ; i < isSgAsmBlock(blocks)->get_statementList().size(); i++   )
    {
      SgAsmFunctionDeclaration* funcDecl = isSgAsmFunctionDeclaration(isSgAsmBlock(blocks)->get_statementList()[i]);

      ROSE_ASSERT(funcDecl!=NULL);

      addBlocksFromFunctionToGraph(returnGraph, instToNodeMap, funcDecl);

    }
  //Add all edges to the graph

  for( Partitioner::BasicBlockStarts::const_iterator bbItr = bb_starts.begin() ; bbItr != bb_starts.end(); ++bbItr )
  {
    rose_addr_t            addr = bbItr->first;
    std::set<rose_addr_t>  edgesTo = bbItr->second;
    SgGraphNode* to   = instToNodeMap[ isSgAsmBlock(instMap.find(addr )->second->get_parent())->get_address()  ];


    for( std::set<rose_addr_t>::iterator edgeItr = edgesTo.begin(); edgeItr != edgesTo.end(); ++edgeItr )
    {
      //Add calls to current function from another function.


      SgGraphNode* from = instToNodeMap[ isSgAsmBlock(instMap.find(*edgeItr)->second->get_parent() )->get_address() ];

      //Add node when we are creating a full cfg or when we have an interprocedural edge for a functino cfg  
      if( isSgAsmFunctionDeclaration(blocks) == NULL || ( from != NULL && to != NULL ) )
      {
        ROSE_ASSERT( from != NULL );
        ROSE_ASSERT( to   != NULL );
        returnGraph->addDirectedEdge( from, to  , " ");

      }

    }

  };

  return returnGraph;
};

/***************************************************************************************************
 * The goal of the function is to create a control flow graph.
 */
SgIncidenceDirectedGraph* constructCFG( const  Partitioner::FunctionStarts& func_starts, const Partitioner::BasicBlockStarts& bb_starts,const Disassembler::InstructionMap& instMap )
{
  SgIncidenceDirectedGraph *returnGraph = new SgIncidenceDirectedGraph();
  ROSE_ASSERT (returnGraph != NULL);

  typedef std::map<rose_addr_t, SgGraphNode*> InstructionToNode;

  InstructionToNode instToNodeMap;

  //Add all nodes to the graph


  //Need to create a complete list of basic blocks as some basic blocks without incoming edges do not show up 
  //in bb_starts
  for( Partitioner::BasicBlockStarts::const_iterator bbItr = bb_starts.begin() ; bbItr != bb_starts.end(); ++bbItr )
  {
    rose_addr_t            addr = bbItr->first;

    std::set<rose_addr_t>  edgesTo = bbItr->second;
  SgAsmInstruction*      inst = instMap.find(addr )->second;

    //FIXME: current fast hack to label node with the function name if BB is a function. Should be done in
    //dot generation mechanism instead.
    Partitioner::FunctionStarts::const_iterator funcItr = func_starts.find( addr );
    std::string functionName = ( funcItr != func_starts.end() ? funcItr->second.name  : unparseInstructionWithAddress(inst) );

    SgGraphNode* node = new SgGraphNode( functionName);
    node->set_SgNode(inst);
    returnGraph->addNode(node);

    instToNodeMap[addr] = node;

  }


  //Add all edges to the graph

  std::vector<rose_addr_t> bb_starts_vec;
  for( Partitioner::BasicBlockStarts::const_iterator bbItr_edge =  bb_starts.begin() ; bbItr_edge != bb_starts.end(); ++bbItr_edge )
  {
    bb_starts_vec.push_back(bbItr_edge->first);
  }


  for( Partitioner::BasicBlockStarts::const_iterator bbItr = bb_starts.begin() ; bbItr != bb_starts.end(); ++bbItr )
  {
    rose_addr_t            addr = bbItr->first;
    std::set<rose_addr_t>  edgesTo = bbItr->second;

    for( std::set<rose_addr_t>::iterator edgeItr = edgesTo.begin(); edgeItr != edgesTo.end(); ++edgeItr )
    {
      //Add calls to current function from another function.


      //Do a binary search for the basic block which contains this address
      rose_addr_t key = *edgeItr;
      unsigned int first = 0, last = bb_starts_vec.size()-1;
      unsigned int mid=0;
      while (first <= last) {
        mid = (first + last) / 2;  // compute mid point.
        if (key > bb_starts_vec[mid]) 
          first = mid + 1;  // repeat search in top half.
        else if (key < bb_starts_vec[mid]) 
          last = mid - 1; // repeat search in bottom half.
        else
          break;     // found it. return position /////
      }

      if(mid>0 && mid < bb_starts_vec.size()-1)
        ROSE_ASSERT( key < bb_starts_vec[mid+1] && key > bb_starts_vec[mid-1]);


      SgGraphNode* from = instToNodeMap[ bb_starts_vec[mid] ];

      SgGraphNode* to   = instToNodeMap[ addr ];
      ROSE_ASSERT( from != NULL );
      ROSE_ASSERT( to   != NULL );

      returnGraph->addDirectedEdge( from, to  , " ");

    }

  }


  return returnGraph;
};


/****************************
 * The goal of this function is to create a binary control flow graph. It does this by construcing a CFG and then finding the
 * subset of the CFG that is a call graph.
 */
SgIncidenceDirectedGraph* constructCallGraph( const  Partitioner::FunctionStarts& func_starts,const Partitioner::BasicBlockStarts& bb_starts, const Disassembler::InstructionMap& instMap  )
{
  Partitioner::BasicBlockStarts func_bb_starts;

  //Check assumption that the functions are sorted according to address in file.
  rose_addr_t last_addr = func_starts.begin()->first;
  for( Partitioner::FunctionStarts::const_iterator funcItr = func_starts.begin() ; funcItr != func_starts.end(); ++funcItr )
        {
          ROSE_ASSERT(last_addr <= funcItr->first);
          last_addr = funcItr->first;

        }

  std::vector<rose_addr_t> func_starts_vec;
  for( Partitioner::FunctionStarts::const_iterator bbItr_edge =  func_starts.begin() ; bbItr_edge != func_starts.end(); ++bbItr_edge )
  {
    func_starts_vec.push_back(bbItr_edge->first);
  }


  //Pair function starts with all functions that has at least on call to it.
  for( Partitioner::BasicBlockStarts::const_iterator bbItr = bb_starts.begin() ; bbItr != bb_starts.end(); ++bbItr )
  {
    rose_addr_t            addr = bbItr->first;
    std::set<rose_addr_t>  edgesTo = bbItr->second;


    if( func_starts.find(addr) != func_starts.end()  ) //If it is a function start iterate over edges to it
    {
      std::set<rose_addr_t> addrSet;

      for( std::set<rose_addr_t>::iterator edgeItr = edgesTo.begin(); edgeItr != edgesTo.end(); ++edgeItr )
      {
    
        //Add calls to current function from another function.
        //First do a binary search for the function that contains this call
        rose_addr_t key = *edgeItr;
        unsigned int first = 0, last = func_starts_vec.size()-1;
        unsigned int mid = 0;
        while (first <= last) {
          mid = (first + last) / 2;  // compute mid point.
          if (key > func_starts_vec[mid]) 
            first = mid + 1;  // repeat search in top half.
          else if (key < func_starts_vec[mid]) 
            last = mid - 1; // repeat search in bottom half.
          else
            break;     // found it. return position /////
        }

        if(mid>0 && mid < func_starts_vec.size()-1)
          ROSE_ASSERT( key < func_starts_vec[mid+1] && key > func_starts_vec[mid-1]);

        addrSet.insert(func_starts_vec[mid]);

      }
      func_bb_starts[addr] = addrSet;

    }
  }

  //Construct a callgraph using the selected subset of the CFG.
  return constructCFG(func_starts, func_bb_starts, instMap);
};


/****************************
 * The goal of this function is to create a binary control flow graph.  
 */
SgIncidenceDirectedGraph* constructCallGraph( SgAsmInterpretation *interp )
{
  /* Get a copy of the disassembler so we can adjust settings locally. */
  Disassembler *d = Disassembler::lookup(interp)->clone();
  d->set_search( Disassembler::SEARCH_DEFAULT  );

  /* RPM: you don't actually need to pass "bad", which returns the addresses/error messages for instructions that
   *      couldn't be disassembled.  Just d->disassembleInterp(interp). */
  /* Disassemble instructions, linking them into the interpretation */
  Disassembler::BadMap bad;
  Disassembler::InstructionMap instMap = d->disassembleInterp(interp, NULL, &bad);


  /* Create the CFG */
  Partitioner part; 

  /* RPM: BasicBlockStarts is contains the callers, not the callees.  It's a map where the keys (first element of each
   *      pair) are the address of the first instruction of each basic block and the values (second element of each
   *      pair) are the virtual addresses of instructions that branch to that basic block.  For instance, if you have
   *          0x3000: JMP 0x5000
   *                  ...
   *          0x4000: CALL 0x5000
   *                  ...
   *          0x5000: NOP
   *      then the BasicBlockStarts will contain the pair (0x5000, [0x3000, 0x4000]). */
  //BasicBlockStarts is a basic block together with a set of know callees


  Partitioner::BasicBlockStarts bb_starts = part.detectBasicBlocks(instMap); //The CFG 
  Partitioner::FunctionStarts func_starts = part.detectFunctions(interp, instMap, bb_starts);


  // SgAsmBlock* blocks = part.partition(interp->get_header(), instMap);
  delete d;
  return constructCallGraph(func_starts,bb_starts,instMap);

};




/****************************
 * The goal of this function is to create a binary control flow graph. It does this by construcing a CFG and then finding the
 * subset of the CG that is a call graph.
 */
SgIncidenceDirectedGraph* constructCFG( SgAsmInterpretation *interp )
{
  /* Get a copy of the disassembler so we can adjust settings locally. */
  Disassembler *d = Disassembler::lookup(interp)->clone();
  d->set_search( Disassembler::SEARCH_DEFAULT  );

  /* RPM: you don't actually need to pass "bad", which returns the addresses/error messages for instructions that
   *      couldn't be disassembled.  Just d->disassembleInterp(interp). */
  /* Disassemble instructions, linking them into the interpretation */
  Disassembler::BadMap bad;
  Disassembler::InstructionMap instMap = d->disassembleInterp(interp, NULL, &bad);


  /* Create the CFG */
  Partitioner part; 

  /* RPM: BasicBlockStarts is contains the callers, not the callees.  It's a map where the keys (first element of each
   *      pair) are the address of the first instruction of each basic block and the values (second element of each
   *      pair) are the virtual addresses of instructions that branch to that basic block.  For instance, if you have
   *          0x3000: JMP 0x5000
   *                  ...
   *          0x4000: CALL 0x5000
   *                  ...
   *          0x5000: NOP
   *      then the BasicBlockStarts will contain the pair (0x5000, [0x3000, 0x4000]). */
  //BasicBlockStarts is a basic block together with a set of know callees


  Partitioner::BasicBlockStarts bb_starts = part.detectBasicBlocks(instMap); //The CFG 
  Partitioner::FunctionStarts func_starts = part.detectFunctions(interp, instMap, bb_starts);


  // SgAsmBlock* blocks = part.partition(interp->get_header(), instMap);
  delete d;
  return constructCFG(func_starts,bb_starts,instMap);

};


