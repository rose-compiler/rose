
#include "rose.h"
#include "CycleDetection.h"
#include "string.h"
#include "stringify.h"

using namespace std;
using namespace RoseBin_DataTypes;
using namespace RoseBin_OS_VER;
using namespace RoseBin_Arch;

bool 
CycleDetection::checkIfValidCycle(SgGraphNode* node_n,
			      SgGraphNode* next_n){
  // traverse the graph from next to node
  bool foundCycle=false;
  std::vector<SgGraphNode*> successors_f;
  std::set<SgGraphNode*> visited_f;
  vector<SgGraphNode*> worklist;
  worklist.push_back(next_n);
  visited_f.insert(next_n);
  while (!worklist.empty()) {
    SgGraphNode* current = worklist.back();
    worklist.pop_back();
    successors_f.clear();
    if (debug)
      std::cerr << "current node " << current << " worklist size : " << 
	worklist.size() << "  visited size : " << visited_f.size() << std::endl;

    vizzGraph->getSuccessors(current, successors_f);    
    vector<SgGraphNode*>::iterator succ = successors_f.begin();
    for (;succ!=successors_f.end();++succ) {
      SgGraphNode* next = *succ;      
      if (debug)
	std::cerr << "worklist:  next node " << next << std::endl;
      if (sameParents(current,next)) { 
	if (next==next_n) {
	  // we have found a cycle
	  foundCycle=true;
	  worklist.clear();
	  if (debug)
	    std::cerr << "worklist: reached origin; abort. " <<  std::endl;
	  break;
	} else {
	  // cycle not found. If this node is in the same function
	  // and it has not been visited before, then we put it in 
	  // the worklist
	  std::set<SgGraphNode*>::iterator 
	    it =visited_f.find(next);
	  if (it==visited_f.end()) {
	    worklist.push_back(next);
	    visited_f.insert(next);
	    if (debug)
	      std::cerr << "worklist: push_back next node. " <<  std::endl;
	  } else {	
	    if (debug)
	      std::cerr << "worklist: no cycle. " <<  std::endl;
	  }

	}
      } else {
	if (debug)
	  std::cerr << "worklist:  different parents "  << std::endl;
      }
    } // for
  } // while
  return foundCycle;
}

bool 
CycleDetection::run(string& name, SgGraphNode* node,
			      SgGraphNode* previous){
  // check known function calls and resolve variables
  ROSE_ASSERT(node);

  //cerr << " cycledetection->run " << node->get_name() << endl;
  SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(node->get_SgNode());
  if (func) {
    // if the node is a function, we clear the visited nodes
    // this should speed up our search
    visited.clear();
    return false;
  }
  successors.clear();
  ROSE_ASSERT(vizzGraph);
  vizzGraph->getSuccessors(node, successors);    
  vector<SgGraphNode*>::iterator succ = successors.begin();
  for (;succ!=successors.end();++succ) {
    // for each successor do...
    SgGraphNode* next = *succ;
    // if the node is an instruction, we check if it was visited
    // if not, we add it to the visited set, otherwise a cycle is present
    std::set<SgGraphNode*>::iterator it =visited.find(next);
    if (it!=visited.end()) {
      // found this node in visited list
      SgAsmx86Instruction* nodeSg = isSgAsmx86Instruction(node->get_SgNode());
      SgAsmx86Instruction* nextSg = isSgAsmx86Instruction(next->get_SgNode());
      if (debug) {
      std::cerr << "Found possible cycle between  " << 
	stringifyX86InstructionKind(nodeSg->get_kind()) << " (" << 
	RoseBin_support::HexToString(nodeSg->get_address()) << ") and " <<
	stringifyX86InstructionKind(nextSg->get_kind()) << " (" << 
	RoseBin_support::HexToString(nextSg->get_address()) << ")" << std::endl;
      }

      bool validCycle = checkIfValidCycle(node,next);
      if (validCycle) {
	if (debug)
	std::cerr << "This is a valid cycle. " << std::endl;
	cycleFound[node]=next;
      } else {
	if (debug)
	  std::cerr << "This is not a cyclic node "  << std::endl;
      }
    }
  }
  visited.insert(node);
  return false;
}


extern "C" BC_GraphAnalysisInterface* create(GraphAlgorithms* algo) {
  return new CycleDetection(algo);
}

