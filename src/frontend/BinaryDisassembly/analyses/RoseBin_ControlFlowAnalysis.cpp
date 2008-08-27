/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 6Jun07
 * Decription : Interface to compare src Tree against
 *              binary Tree
 * todo : color the variables and names in the src/bin tree
 ****************************************************/

#include "rose.h"
#include "RoseBin_ControlFlowAnalysis.h"

using namespace std;

void
RoseBin_ControlFlowAnalysis::getCFGNodesForFunction(std::set<SgDirectedGraphNode*>& visited_f,
						    std::set<std::string>& visited_names,
						    SgDirectedGraphNode* next_n, std::string nodeName){
  // traverse the graph from next to node
  std::vector<SgDirectedGraphNode*> successors_f;
  //  std::set<SgDirectedGraphNode*> visited_f;
  //std::set<std::string> visited_names;
  vector<SgDirectedGraphNode*> worklist;
  worklist.push_back(next_n);
  visited_f.insert(next_n);
  visited_names.insert(nodeName);
  while (!worklist.empty()) {
    SgDirectedGraphNode* current = worklist.back();
    worklist.pop_back();
    successors_f.clear();

    vizzGraph->getSuccessors(current, successors_f);    
    vector<SgDirectedGraphNode*>::iterator succ = successors_f.begin();
    for (;succ!=successors_f.end();++succ) {
      SgDirectedGraphNode* next = *succ;      
      if (sameParents(current,next)) { 
	std::set<SgDirectedGraphNode*>::iterator 
	  it =visited_f.find(next);
	if (it==visited_f.end()) {
	  worklist.push_back(next);
	  visited_f.insert(next);
	  SgNode* internal = next->get_SgNode();
	  SgAsmInstruction* inst = isSgAsmInstruction(internal);
	  if (inst) {
	    string name = RoseBin_support::HexToString(inst->get_address());
	    //cerr << " adding node to function : ."<<name<<"."<<endl;
	    visited_names.insert(name);
	  }
	} 
      }
    } // for
  } // while
}


void RoseBin_ControlFlowAnalysis::printGraph(std::string fileName, std::set<std::string>& filter) {
  std::set<std::string>::const_iterator it = filter.begin();
  for (;it!=filter.end();++it) {
    std::cerr << "CFG -- contains filter: ." << *it << "." << endl;
  }

  typedef rose_hash::hash_map <std::string, SgDirectedGraphNode*> nodeType;
  nodeType result;
  nodeType nodes = vizzGraph->nodes;
  nodeType::iterator itn2 = nodes.begin();
  for (; itn2!=nodes.end();++itn2) {
    string hex_address = itn2->first;

    SgDirectedGraphNode* node = itn2->second;
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    if (func) {
      std::cerr << "ControlFlowAnalysis:: found function: ." << hex_address << "." <<endl;
      std::set<std::string>::const_iterator it = filter.find(hex_address);
      if (it!=filter.end()) {
	//std::cerr << " ******************* match ********************* " << std::endl;
	set<SgDirectedGraphNode*> gns;
	set<std::string> names;
	getCFGNodesForFunction(gns,names,node,hex_address);
	//cerr << " nodes in function: " << gns.size() << " " << names.size() <<endl;
	ROSE_ASSERT(gns.size()==names.size());
	set<SgDirectedGraphNode*>::const_iterator it2 = gns.begin();
	set<std::string>::const_iterator it3 = names.begin();
	for (;it2!=gns.end();++it2, ++it3) {
	  std::string name = *it3;
	  SgDirectedGraphNode* n = *it2;
	  //cerr << " adding to result ."<<name<<"."<<endl; 
	  result[name]=n;
	}

      }
    }
  }
    vizzGraph->nodes=result;
  
  // create file
  bool forward_analysis=true;
  bool multiedge=false;
  std::ofstream myfile;
  myfile.open(fileName.c_str());
  
  string name = "ROSE Graph";
  vizzGraph->printProlog(myfile, name);

  string functionName="";

  vizzGraph->setGrouping(true);
  vizzGraph->printNodes(true, this, forward_analysis, myfile,functionName);
  nrNodes=vizzGraph->nodes.size();

  vizzGraph->printEdges(this,myfile, multiedge);
  nrEdges=vizzGraph->edges.size();

  vizzGraph->printEpilog(myfile);
  myfile.close();  
}

/****************************************************
 * run the compare analysis
 ****************************************************/
void RoseBin_ControlFlowAnalysis::run(RoseBin_Graph* vg, string fileN, bool multiedge) {
  vizzGraph = vg;
  fileName = fileN;
  vizzGraph->nodes.clear();
  vizzGraph->edges.clear();

  double start=0;
  double ends=0;

  func_nr=0;
  //  ROSE_ASSERT(roseBin);

  if (RoseBin_support::DEBUG_MODE_MIN()) 
    cerr << "\n **********************  running ControlFlowAnalysis ... " << fileName << endl;
  /*
  // check if graph exists, if yes, get it out, otherwise create
  __gnu_cxx::hash_map <std::string, SgDirectedGraph*>::iterator itG = graphs.find(analysisName);
  if (itG!=graphs.end()) {
  // note: this does currently not work because graph and nodes are disconnected (Grammar problem)
  vizzGraph->graph = itG->second;
  cerr << " >>> found existing dfa graph ... using that one. " << endl;
  } else {
  */
  vizzGraph->graph   = new SgDirectedGraph(analysisName,analysisName);
  
  nr_target_missed=0;
  if (RoseBin_support::DEBUG_MODE_MIN()) 
    cerr << " running ControlFlowAnalysis ... " << endl;
  start = RoseBin_support::getTime();
  this->traverse(globalBin,preorder);
  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " CFG runtime : " << (double) (ends - start)   << " sec" << endl;
    cerr << " Number of targets missed : " << nr_target_missed << endl;
  }
  //  graphs[analysisName] =vizzGraph->graph;
  //}

  //graph->set_nodes(nodes);
  //graph->set_edges(edges);
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " ********************** done running ControlFlowAnalysis ... " << endl;
    cerr << " ********************** saving to file ... " << endl;
  }

  createInstToNodeTable();

  // create file
  std::ofstream myfile;
  myfile.open(fileName.c_str());

  string name = "ROSE Graph";
  vizzGraph->printProlog(myfile, name);

  string funcName="";
  start = RoseBin_support::getTime();
  vizzGraph->setGrouping(true);
  vizzGraph->printNodes(false,this, forward_analysis, myfile,funcName);
  nrNodes=vizzGraph->nodes.size();
  //vizzGraph->nodes.clear();

  if (printEdges) 
    vizzGraph->printEdges(this,myfile, multiedge);
  nrEdges=vizzGraph->edges.size();
  //vizzGraph->edges.clear();

  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN()) 
    cerr << " CFG runtime : " << (double) (ends - start)   << " sec" << endl;

  vizzGraph->printEpilog(myfile);
  myfile.close();  

#if 0
  set<std::string> partialCFG;
  partialCFG.insert(" 8048363_f");
  partialCFG.insert(" 804828f_f");
  printGraph("thomas.dot",partialCFG);
#endif
}

