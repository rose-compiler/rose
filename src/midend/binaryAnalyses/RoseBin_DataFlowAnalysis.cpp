/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/
// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "RoseBin_DataFlowAnalysis.h"

#include "dataflowanalyses/InterruptAnalysis.h"

using namespace std;
#include <cstdlib>

std::set < SgGraphNode* >
RoseBin_DataFlowAnalysis::getDefFor(SgGraphNode* node, std::pair<X86RegisterClass, int> initName) {
  return defuse->getDefFor(node, initName);
}

std::set < SgGraphNode* >
RoseBin_DataFlowAnalysis::getDefFor( uint64_t inst, std::pair<X86RegisterClass, int> initName) {
  SgGraphNode* node = getNodeFor(inst);
  return defuse->getDefFor(node, initName);
}


std::set < uint64_t >
RoseBin_DataFlowAnalysis::getDefForInst( uint64_t inst, std::pair<X86RegisterClass, int> initName) {
  std::set <uint64_t> hexSet;
  SgGraphNode* node = getNodeFor(inst);
  if (node==NULL)
    cerr << "ERROR: getDefForInst " << RoseBin_support::HexToString(inst) << "  does not exist! " << endl;

  set<SgGraphNode*> nodes = defuse->getDefFor(node, initName);
  set<SgGraphNode*>::iterator it = nodes.begin();
  for (;it!=nodes.end();++it) {
    SgGraphNode* n = *it;
    if (n) {
      SgAsmInstruction* instNode = isSgAsmInstruction(n->get_SgNode());
      if (instNode) {
	hexSet.insert(instNode->get_address());
	//cerr << "INSERT: getDefForInst " <<
	//RoseBin_support::HexToString(instNode->get_address()) << endl;
      }
    }
  }
  return hexSet;
}




bool
RoseBin_DataFlowAnalysis::containsHash( nodeHashSetType& vec,
					SgGraphNode* node) {
   nodeHashSetType:: const_iterator it = vec.find(node);
  if (it!=vec.end())
    return true;
  return false;
}


void RoseBin_DataFlowAnalysis::init() {
  // stores the visited nodes
  visited.clear();
  visitedCounter.clear();
  nodeBeforeMap.clear();
  nrOfNodesVisited=0;
}

void
RoseBin_DataFlowAnalysis::traverseEdges(RoseBin_DataFlowAbstract* analysis) {
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " >> Traversing over all edges and adding label ... " << endl;
  //typedef rose_graph_integer_edge_hash_multimap edgesM;
  rose_graph_integer_edge_hash_multimap edges = vizzGraph->get_node_index_to_edge_multimap_edgesOut();
  rose_graph_integer_edge_hash_multimap::const_iterator it = edges.begin();
  for (;it!=edges.end();++it) {
     int index  = it->first;
    SgGraphNode* src = NULL;
    rose_graph_integer_node_hash_map::iterator nIT = vizzGraph->get_node_index_to_node_map().find(index);
    if (nIT!=vizzGraph->get_node_index_to_node_map().end())
      src=nIT->second;
    ROSE_ASSERT(src);
    //   SgGraphNode* src = it->first;
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it->second);
    SgGraphNode* trg = NULL;
    if (edge)
      trg = isSgGraphNode(edge->get_to());
    if (g_algo->isValidCFGEdge(trg, src))
      analysis->runEdge(src,trg);
  }
}

void
RoseBin_DataFlowAnalysis::traverseNodes(RoseBin_DataFlowAbstract* analysis) {
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " >> Traversing over all nodes and adding label ... " << endl;
  rose_graph_integer_node_hash_map::iterator itn = vizzGraph->get_node_index_to_node_map().begin();
  for (; itn!=vizzGraph->get_node_index_to_node_map().end();++itn) {
    SgGraphNode* node = isSgGraphNode(itn->second);
    ROSE_ASSERT(node);
    SgNode* internal = node->get_SgNode();
    ROSE_ASSERT(internal);
    SgAsmInstruction* inst = isSgAsmInstruction(internal);
    if (inst) {
      uint64_t address = inst->get_address();
      RoseBin_Variable* var = analysis->getVariable(address);
      if (var) {
	std::string var_str = var->toString();
	node->append_properties(SgGraph::variable,var_str);
      }
    }
  }
}

bool
RoseBin_DataFlowAnalysis::existsPath(SgGraphNode* start, SgGraphNode* end) {
  // make sure its not a SgAsmCall and the next node is a DirectedControlFlowEdge
  ROSE_ASSERT(g_algo->info);
  bool exists = false;
  ROSE_ASSERT(start);
  ROSE_ASSERT(end);
  SgAsmx86Instruction* next = isSgAsmx86Instruction(start);
  SgAsmx86Instruction* endAsm = isSgAsmx86Instruction(end);
  if (next && endAsm) {
    while (next!=endAsm) {
      next = isSgAsmx86Instruction(next->cfgBinFlowOutEdge(g_algo->info));
      if (next==NULL)
	break;
      if ((next->get_kind() == x86_call || next->get_kind() == x86_ret) && next!=endAsm)
	break;
    }
    exists = true;
  }
  return exists;
}

bool
RoseBin_DataFlowAnalysis::exceptionCall(SgAsmx86Instruction* call) {
  // this function returns true, if the function that is being called is the _malloc function
  // this is good to know, so that the malloc analysis can be performed even if there is no ret

  // (tps - 05/23/08): Since the new disassembler does not know function names, this analysis
  // does not work.
  // todo : as long as there are no function names -- the malloc analysis will not work.
  bool exception=false;
  if (call==NULL)
    return exception;
  if (call->get_kind() != x86_call)
    return exception;
  SgAsmOperandList* opList = call->get_operandList();
  ROSE_ASSERT(opList);
  SgAsmExpressionPtrList ptrList = opList->get_operands();
  // get the first (and only) element
  string comment = call->get_comment();
  if (ptrList.size()!=0) {
    SgAsmExpression* expr = *(ptrList.begin());
    string replace = expr->get_replacement();
    if (replace=="_malloc" || replace=="malloc@plt"
	|| comment=="malloc")
      exception=true;
  }
  //  cerr << "Found call --- comment = " << comment << "  exception = " << exception << endl;
  return exception;
}

/***********************************************************************
 * (10/31/07) tps: Traverses the graph for each node in rootNodes
 * and applies to each node the evaluate function
 * which can be either def_use, variable detection or emulation
 * Each node in the controlflow of rootNode is traversed (forward)
 * and only if the hasChanged function returns false, the algorithm
 * comes to a fixpoint
 ***********************************************************************/
void
RoseBin_DataFlowAnalysis::traverseGraph(vector <SgGraphNode*>& rootNodes,
					RoseBin_DataFlowAbstract* analysis,
					bool interprocedural){
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " traverseGraph : debug: " << RoseBin_support::resBool(RoseBin_support::DEBUG_MODE()) <<
      "  debug_min : " <<  RoseBin_support::resBool(RoseBin_support::DEBUG_MODE_MIN()) << endl;
  // Number of functions traversed
  int funcNr =0;
  // ---------------------------------------------------------------------
  // stores the nodes that still needs to be visited
  //  vector<SgGraphNode*> worklist;
  deque<SgGraphNode*> worklist;
  nodeHashSetType worklist_hash;
  // a vector of successors of the current node
  vector<SgGraphNode*> successors;
  // ---------------------------------------------------------------------


  // iterate through all functions
  vector<SgGraphNode*>::iterator it = rootNodes.begin();
  for (; it!=rootNodes.end();++it) {
    // current node
    SgGraphNode* node = *it;

    string func_name = vizzGraph->getProperty(SgGraph::name, node);
    RoseBin_support::checkText(func_name);
    funcNr++;
    if (RoseBin_support::DEBUG_MODE()) {
      cout << "\n\n -----------  dataflow analysis of function ("+RoseBin_support::ToString(funcNr)+"/"+
	RoseBin_support::ToString(rootNodes.size())+") : " << func_name <<
	"  visited size : " << visited.size() <<
	"  total visited nodes : " << nrOfNodesVisited << endl;
      // debug
    }
    if (RoseBin_support::DEBUG_MODE_MIN()) {
      cerr << " -----------  dataflow analysis of function ("+RoseBin_support::ToString(funcNr)+"/"+
	RoseBin_support::ToString(rootNodes.size())+") : " << func_name <<
	"  visited size : " << visited.size() <<
	"  total visited nodes : " << nrOfNodesVisited <<
	"  def size  : " << analysis->getDefinitionSize() << endl;
    }

    // indicates whether the current value for this node has changed
    bool hasChanged=false;
    // pushback into worklist and visited list
    worklist.push_back(node);
    worklist_hash.insert(node);
    visited.insert(node);
    visitedCounter[node] = 1;
    vector <SgGraphNode*> pre;
    // while there are still graph nodes in the worklist do

    while (worklist.size()>0) {
      nrOfNodesVisited++;
      // the new node is taken from the back of the worklist
      //node = worklist.back();
      //worklist.pop_back();
      node = worklist.front();
      worklist.pop_front();

      worklist_hash.erase(node);
      // get the successors of the current node and store in successors vector
      string name = vizzGraph->getProperty(SgGraph::name, node);

      //if (RoseBin_support::DEBUG_MODE_MIN() && node)
      //	if (node->get_SgNode())
      //  cerr << node->get_SgNode()->class_name() << "  " << node << "  " << node->get_name() << endl;

      if (RoseBin_support::DEBUG_MODE_MIN() && node) {
	SgAsmInstruction* instr = isSgAsmInstruction(node->get_SgNode());
	if (instr) {
	  SgAsmFunctionDeclaration* funcParent = isSgAsmFunctionDeclaration(instr->get_parent());
	  if (funcParent) {
	    string parent = funcParent->get_name();
	    cout << " ---- analysis of node in function : " << parent <<
	      "  defs " << analysis->getDefinitionSize() <<
	      " visited : " << RoseBin_support::ToString(visitedCounter[node]) << endl;
	  }
	}
      }


      if (RoseBin_support::DEBUG_MODE())
	cout << "\n evaluating: " << name << endl;
      // do something with the current node
      // e.g. checkVariables(name, node);
      SgGraphNode* nodeBefore= NULL;
      BeforeMapType::const_iterator it =
	nodeBeforeMap.find(node);
      if (it!=nodeBeforeMap.end())
	nodeBefore = it->second;
      // successor vector is empty on each new node
      successors.clear();
      ROSE_ASSERT(isSgIncidenceDirectedGraph(vizzGraph));
      isSgIncidenceDirectedGraph(vizzGraph)->getSuccessors(node, successors);

      hasChanged = analysis->run(name, node, nodeBefore);

      // append the successors to the worklist
      if (RoseBin_support::DEBUG_MODE())
	cout << ">> getting successors  (" << successors.size() << ") for : " << name << endl;
      //	if (successors.size()==0)
      //	  cout << "PROBLEM ..................................................... : " << endl;
      vector<SgGraphNode*>::iterator succ = successors.begin();
      for (;succ!=successors.end();++succ) {
	// for each successor do...
	SgGraphNode* next = *succ;
        SgAsmx86Instruction* nodeN = isSgAsmx86Instruction(node->get_SgNode());
        //if (!nodeN) continue;
        SgAsmx86Instruction* nextN = isSgAsmx86Instruction(next->get_SgNode());
        //if (!nextN) continue;

	string name_n = vizzGraph->getProperty(SgGraph::name, next);



	bool call = false;
	bool exceptionCallNext = false;
	if (nextN)
	  exceptionCallNext = exceptionCall(nextN->get_kind() == x86_call ? nextN : 0);
	bool exceptionCallNode = false;
	if (nodeN)
	  exceptionCallNode = exceptionCall(nodeN->get_kind() == x86_call ? nodeN : 0);
	if (RoseBin_support::DEBUG_MODE())
	  std::cout << " exceptionCallNode : " << exceptionCallNode << " exceptionCallNext : " << exceptionCallNext << endl;
	// if function call is call to malloc we have an exception and follow the call path
	if ((exceptionCallNode && !exceptionCallNext)) {
        } else if (
                   //if (
                   (nodeN && nodeN->get_kind() == x86_call) ||
                   (nextN && nextN->get_kind() == x86_ret) )
          call = true;
        //bool sameParent = analysis->sameParents(node, next);

	bool validNode=false;
	if (g_algo->isValidCFGEdge(next, node) || exceptionCallNode)
	  validNode = true;

	// debug ------------------------
	if (RoseBin_support::DEBUG_MODE()) {
	  string nodeBeforeStr="";
	  if (nodeBefore) nodeBeforeStr= nodeBefore->get_name();
	  cout << "  DEBUG : >>>>>>>> previous node " << nodeBeforeStr
               << "      This node : " << name << "  next node : " << name_n
	       << "  ** validNode : " << RoseBin_support::resBool(validNode) << endl;
        }


	// ----------------------------------
	if (( interprocedural==false && !call) //
	    ||  (interprocedural==true && validNode)) {
	  if (visited.find(next)==visited.end()) {
	    // if the successor is not yet visited
	    // mark as visited and put into worklist
	    if (RoseBin_support::DEBUG_MODE())
	      cout << " never visited next node before... " << name_n <<
		" interprocedural : " << interprocedural << "  call : " << call << endl;
	    if (RoseBin_support::DEBUG_MODE())
	      cout << "adding to visited : " << name_n << endl;

	    visited.insert(next);
	    nodeBeforeMap[next]=node;
	    visitedCounter[next]=1;
	    vizzGraph->setProperty(SgGraph::visitedCounter, next, RoseBin_support::ToString(1));
	    if (!containsHash(worklist_hash,next)) {
	      // add next node only if the next node
	      if (RoseBin_support::DEBUG_MODE())
		cout << "adding to worklist: " << name_n << endl;
	      worklist.push_back(next);
	      worklist_hash.insert(next);
	    }
	  } else {
	    // if the successor has been visited, we need to check if it has changed
	    // if it has not, we continue, else we need to push it back to the worklist
	    int nr = visitedCounter[next];
	    if (RoseBin_support::DEBUG_MODE())
	      cout << " visited next node before... " << RoseBin_support::ToString(nr) <<
		"  Changed == " << RoseBin_support::resBool(hasChanged) << endl;

	    if (hasChanged) {
	      visitedCounter[next]=++nr;
	      vizzGraph->setProperty(SgGraph::visitedCounter, next, RoseBin_support::ToString(nr));
	      if (RoseBin_support::DEBUG_MODE())
		cout << " has changed : " << RoseBin_support::resBool(hasChanged) <<
		  "  -- interprocedural : " << RoseBin_support::resBool(interprocedural) <<
		  "  -- Call : " << RoseBin_support::resBool(call) <<
		  "  ------> new number: " << RoseBin_support::ToString(nr) <<
		  "  -- contained in hash? : " << RoseBin_support::resBool(containsHash(worklist_hash,next)) <<
		  "  ---- nr of Defs: " << RoseBin_support::ToString(analysis->getDefinitionSize()) <<
		  "  ---- nr of Use: " << RoseBin_support::ToString(analysis->getUsageSize())
                     << endl;

	      if (interprocedural || (!interprocedural && !call)){ //sameParent)) { //!call && ) {
		if (!containsHash(worklist_hash,next)) {
		  worklist_hash.insert(next);
		  worklist.push_back(next);
		  if (RoseBin_support::DEBUG_MODE())
		    cout << " adding to worklist: " << name_n << endl;
		}
	      }
	    } else
	      if (RoseBin_support::DEBUG_MODE())
		cout << " has NOT changed. " << endl;
	    //else we continue with the next node
	  }
	}
      } // for
    } // while worklist.size()>0

  } // for rootNodes
}

void RoseBin_DataFlowAnalysis::writeToFile(bool w) {
  writeFile=w;
}

/****************************************************
 * run the compare analysis
 ****************************************************/
void RoseBin_DataFlowAnalysis::run(RoseBin_Graph* vg, string fileN, bool multiedge) {

  vizzGraph=vg;
  fileName=fileN;
  double start=0;
  double ends=0;
  clearMaps();

  func_nr=0;
  //  ROSE_ASSERT(roseBin);

  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << "\n ********************** running DataFlowAnalysis ... " << fileName << endl;

  // create a new Dataflow Graph
  vizzGraph->graph   = new SgIncidenceDirectedGraph(analysisName);
  nr_target_missed=0;
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " running CFG in DataFlowAnalysis ... " << endl;
  start = RoseBin_support::getTime();
  // create the  (cfg) graph
  this->traverse(globalBin,preorder);
  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " CFG (DFG) runtime : " << (double) (ends - start)   << " sec" << endl;
    cerr << " Number of targets missed : " << nr_target_missed << endl;
  }

  createInstToNodeTable();

  // get the rootnodes
  vector<SgGraphNode*> rootNodes;
  getRootNodes(rootNodes);
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " collecting root nodes for dfa ... " ;
    cerr << " found root nodes : " << rootNodes.size() << endl;
  }


  // defuse analysis  ----------------------------------------------
  // clear def and use tables in defuse algo
  defuse->init(vizzGraph);
  // clear tables in graph-algo (visited, etc.)
  init();

  // traverse it once intraprocedural
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " Running the DATAFLOW-ANALYSIS  (interprocedural) : " <<
      RoseBin_support::resBool(interprocedural)  <<endl;

  start = RoseBin_support::getTime();
  traverseGraph(rootNodes, defuse, interprocedural);
  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " DFG ANALYSIS runtime : " << (double) (ends - start)   << " sec" << endl;
  //defuse->copyUseDefTableToInstTable();

  // printDebugDefTableToFile
  //if (RoseBin_support::DEBUG_MODE_MIN()) {
  //  cerr << " Writing DefTable to file " << endl;
  //  defuse->printDefTableToFile(fileN+"_inter_"+RoseBin_support::resBool(interprocedural)+".defTable");
  //}


  // do the edges -- if edges should be labeled for debugging
  if (printEdges) {
    if (RoseBin_support::DEBUG_MODE_MIN())
      cerr << " Writing Edge Labels to Edges " << endl;
    start = RoseBin_support::getTime();
    traverseEdges(defuse);
    ends = RoseBin_support::getTime();
    if (RoseBin_support::DEBUG_MODE_MIN())
      cerr << " DFG Edge annotation runtime : " << (double) (ends - start)   << " sec" << endl;
  }


  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " ... Staring Variable Analysis " << endl;
    cerr << " defuse defsize " << defuse->getDefinitionSize() << endl;
  }

  ROSE_ASSERT(g_algo->info);
  // variable analysis --------------------------------------------------
  variableAnalysis = new RoseBin_VariableAnalysis(defuse,g_algo);
  if (RoseBin_support::DEBUG_MODE_MIN()) 
  cerr << " variableAnalysis defsize " << variableAnalysis->getDefinitionSize() << endl;
  variableAnalysis->init(vizzGraph);
  init();
  traverseGraph(rootNodes, variableAnalysis, interprocedural);

  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " Writing Variable names to nodes " << endl;
  start = RoseBin_support::getTime();
  traverseNodes(variableAnalysis);
  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " DFG Variable annotation runtime : " << (double) (ends - start)   << " sec" << endl;

  // emulation --------------------------------------------------
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " ... Staring Emulation Analysis " << endl;
  // emulation analysis
  ROSE_ASSERT(g_algo->info);
  RoseBin_DataFlowAbstract* emulate = new RoseBin_Emulate(g_algo);
  emulate->init(vizzGraph);
  init();
  //traverseGraph(rootNodes, emulate, interprocedural);


  int nrOfFunc = rootNodes.size();
  int nrOfInst = vizzGraph->get_node_index_to_node_map().size() - nrOfFunc;
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " ***** STATISTICS : " << endl;
    cerr << " Nr of Nodes Visited during DFA : " << nrOfNodesVisited << endl;
    cerr << " Nr of Functions : " << nrOfFunc << endl;
    cerr << " Nr of Instructions : " << nrOfInst << endl;
    double res = 0;
    if (nrOfFunc>0)
      res = nrOfInst / nrOfFunc;
    cerr << " Average Nr of Inst per Function : " << RoseBin_support::ToString(res) << endl;
    cerr << " Nr of Memory writes : " << dynamic_cast<RoseBin_DefUseAnalysis*>(defuse)->getNrOfMemoryWrites() << endl;
    cerr << " Nr of Register writes : " << dynamic_cast<RoseBin_DefUseAnalysis*>(defuse)->getNrOfRegisterWrites() << endl;
    cerr << " Nr of Definitions : " << defuse->getDefinitionSize() << endl;
    cerr << " Nr of Uses : " << defuse->getUsageSize() << endl;


    cerr << " ********************** done running DataFlowAnalysis ... " << endl;
    cerr << " ********************** saving to file ... " << endl;
  }

    nrNodes=vizzGraph->get_node_index_to_node_map().size();
    nrEdges=vizzGraph->get_node_index_to_edge_multimap_edgesOut().size();
    ends = RoseBin_support::getTime();
    if (RoseBin_support::DEBUG_MODE_MIN())
      cerr << " DFG runtime : " << (double) (ends - start)   << " sec" << endl;

  // create file
  if (writeFile) {
    std::ofstream myfile;
    myfile.open(fileName.c_str());

    string name = "ROSE Graph";
    vizzGraph->printProlog(myfile, name);

    string functionName="";
    start = RoseBin_support::getTime();
    vizzGraph->setGrouping(true);
    vizzGraph->printNodes(true, this, forward_analysis, myfile,functionName);

    //  vizzGraph->get_nodes().clear();

    ROSE_ASSERT(g_algo->info);
    vizzGraph->printEdges(g_algo->info, this,myfile, multiedge);

    //vizzGraph->get_edges()->get_edges().clear();


    vizzGraph->printEpilog(myfile);
    myfile.close();
  }

}


