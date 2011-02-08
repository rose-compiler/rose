/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 6Jun07
 * Decription : Interface to compare src Tree against
 *              binary Tree
 * todo : color the variables and names in the src/bin tree
 ****************************************************/
// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "RoseBin_ControlFlowAnalysis.h"
#include "RoseBin_DotGraph.h"
using namespace std;

static bool debug=false;

void
RoseBin_ControlFlowAnalysis::getCFGNodesForFunction(std::set<SgGraphNode*>& visited_f,
                                                    std::set<std::string>& visited_names,
                                                    SgGraphNode* next_n, std::string nodeName){
  // traverse the graph from next to node
  std::vector<SgGraphNode*> successors_f;
  //  std::set<SgGraphNode*> visited_f;
  //std::set<std::string> visited_names;
  vector<SgGraphNode*> worklist;
  worklist.push_back(next_n);
  visited_f.insert(next_n);
  visited_names.insert(nodeName);
  while (!worklist.empty()) {
    SgGraphNode* current = worklist.back();
    worklist.pop_back();
    successors_f.clear();

    vizzGraph->getSuccessors(current, successors_f);
    vector<SgGraphNode*>::iterator succ = successors_f.begin();
    for (;succ!=successors_f.end();++succ) {
      SgGraphNode* next = *succ;

        std::set<SgGraphNode*>::iterator
          it =visited_f.find(next);
        if (sameParents(current,next))
        if (it==visited_f.end()) {
          //      if (sameParents(current,next))
            worklist.push_back(next);
          visited_f.insert(next);
          SgNode* internal = next->get_SgNode();
          SgAsmInstruction* inst = isSgAsmInstruction(internal);
          if (inst) {
            string name = RoseBin_support::HexToString(inst->get_address());
            if (debug)
            cerr << " adding node to function : ."<<name<<"."<<endl;
            visited_names.insert(name);
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

// typedef rose_hash::unordered_map <std::string, SgGraphNode*> nodeType;
// typedef rose_hash::unordered_map <string, SgGraphNode*,hash_stringptr> nodeType;
  rose_graph_integer_node_hash_map result;
  //rose_graph_hash_multimap& nodes = vizzGraph->get_node_index_to_node_map();
  rose_graph_integer_node_hash_map nodes = vizzGraph->get_node_index_to_node_map();
  rose_graph_integer_node_hash_map::iterator itn2 = nodes.begin();
  for (; itn2 != nodes.end();++itn2) {
    //    string hex_address = itn2->first;

    SgGraphNode* node = itn2->second;
    string hex_address = node->get_name();
    //string hex_addr_tmp = node->get_name();
    //ROSE_ASSERT(hex_address==hex_addr_tmp);

    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    if (func) {
      std::cerr << "ControlFlowAnalysis:: found function: ." << hex_address << "." <<endl;
      std::set<std::string>::const_iterator it = filter.find(hex_address);
      if (it!=filter.end()) {
        //std::cerr << " ******************* match ********************* " << std::endl;
        set<SgGraphNode*> gns;
        set<std::string> names;
        getCFGNodesForFunction(gns,names,node,hex_address);
        if (debug)
        cerr << " nodes in function: " << gns.size() << " " << names.size() <<endl;
        ROSE_ASSERT(gns.size()==names.size());
        set<SgGraphNode*>::const_iterator it2 = gns.begin();
        set<std::string>::const_iterator it3 = names.begin();
        for (;it2!=gns.end();++it2, ++it3) {
          std::string name = *it3;
          SgGraphNode* n = *it2;
          if (debug)
          cerr << " adding to result ."<<name<<"."<<endl;
          result.insert(make_pair(itn2->first,n));
        }

      }
    }
  }
  rose_graph_integer_node_hash_map nodesResult = nodes;
  rose_graph_integer_node_hash_map::iterator itn23 = nodes.begin();
  for (; itn23!=nodes.end();++itn23) {
    //string hex_address = isSgGraphNode(itn23->second)->get_name();
    int pos = itn23->first;
    //    rose_graph_integer_node_hash_map::iterator it = result.find(hex_address);
    rose_graph_integer_node_hash_map::iterator it = result.find(pos);
    if (it==result.end()) {
      // not found in result, i.e. delete
      //nodesResult.erase(hex_address);
      nodesResult.erase(pos);
    }
  }
  //  vizzGraph->nodes=nodesResult;

#if 0
  // vizzGraph->nodes=result;

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
  nrEdges=vizzGraph->get_node_index_to_edge_multimap_edgesOut().size();

  vizzGraph->printEpilog(myfile);
  myfile.close();

#endif



#if 1
  RoseBin_Graph* gr = new RoseBin_DotGraph();
  gr->graph = new SgIncidenceDirectedGraph("test");//SgDirectedGraph("test","test");
  gr->get_node_index_to_node_map()=nodesResult;

  //typedef SB_DirectedGraph::edgeType edgeType;
  rose_graph_integer_edge_hash_multimap edges = vizzGraph->get_node_index_to_edge_multimap_edgesOut();
  rose_graph_integer_edge_hash_multimap resultEdges;
  rose_graph_integer_edge_hash_multimap::iterator itEdg = edges.begin();

  for (; itEdg!=edges.end();++itEdg) {
    int index  = itEdg->first;
    SgGraphNode* node = NULL;
    rose_graph_integer_node_hash_map::iterator nIT = vizzGraph->get_node_index_to_node_map().find(index);
    if (nIT!=vizzGraph->get_node_index_to_node_map().end())
      node=nIT->second;
    ROSE_ASSERT(node);
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(itEdg->second);
    SgGraphNode* target = isSgGraphNode(edge->get_to());
    rose_graph_integer_node_hash_map::iterator itn2 = nodesResult.begin();
    bool foundS=false;
    if (node)
      foundS=true;
    bool foundT=false;
    for (; itn2!=nodesResult.end();++itn2) {
      SgGraphNode* n = itn2->second;
      //if (n==source) foundS=true;
      if (n==target) foundT=true;
    }
    if (foundS==false || foundT==false) {

    } else
      resultEdges.insert(make_pair(node->get_index(),edge));
  }

  gr->get_node_index_to_edge_multimap_edgesOut()=resultEdges;

  // create file
  bool forward_analysis=true;
  bool multiedge=false;
  std::ofstream myfile;
  myfile.open(fileName.c_str());

  string name = "ROSE Graph";
  gr->printProlog(myfile, name);

  string functionName="";

  gr->setGrouping(true);
  gr->printNodes(true, this, forward_analysis, myfile,functionName);
  nrNodes=gr->get_node_index_to_node_map().size();
  ROSE_ASSERT(g_algo->info);
  gr->printEdges(g_algo->info, this,myfile, multiedge);
  nrEdges=gr->get_node_index_to_edge_multimap_edgesOut().size();

  gr->printEpilog(myfile);
  myfile.close();
#endif
}

/****************************************************
 * run the compare analysis
 ****************************************************/
void RoseBin_ControlFlowAnalysis::run(RoseBin_Graph* vg, string fileN, bool multiedge) {
  vizzGraph = vg;
  fileName = fileN;
  clearMaps();

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
  vizzGraph->graph   = new SgIncidenceDirectedGraph(analysisName);//
  //SgDirectedGraph(analysisName,analysisName);

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
  nrNodes=vizzGraph->get_node_index_to_node_map().size();
  //vizzGraph->nodes.clear();
  ROSE_ASSERT(g_algo->info);
  if (printEdges)
    vizzGraph->printEdges(g_algo->info, this,myfile, multiedge);
  nrEdges=vizzGraph->get_node_index_to_edge_multimap_edgesOut().size();
  //vizzGraph->get_edges()->get_edges().clear();

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

