/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 17Oct07
 * Description: Creates the binary call graph
 ****************************************************/
 // tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "RoseBin_CallGraphAnalysis.h"

using namespace std;

static bool debug = false;

void
RoseBin_CallGraphAnalysis::findClusterOfNode(SgGraphNode* next_n,
                                             int& currentCluster,
                                             std::map<SgAsmFunctionDeclaration*,int>& visited) {
  int resultsCluster=0;
  std::map<SgAsmFunctionDeclaration*,int>::const_iterator t = visited.begin();
  for (;t!=visited.end();++t) {
    SgAsmFunctionDeclaration* func = t->first;
    int cluster = t->second;
    ROSE_ASSERT(func);
    if (debug)
    cerr << "  ..... contains : " << cluster << "  node : ." << RoseBin_support::HexToString(func->get_address()) << ".  " << endl;
  }

  std::set<SgGraphNode*> curr_cluster;
  // check if this node has been visited before
  SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(next_n->get_SgNode());
  ROSE_ASSERT(func);
  std::map<SgAsmFunctionDeclaration*,int>::const_iterator clust = visited.find(func);
  if (clust!=visited.end()) {
    // found it in clusters
    resultsCluster=clust->second;
    //    visited[func]=resultsCluster;
    return;
  } else {
    curr_cluster.insert(next_n);
    if (debug)
    cerr << "    >>>> RoseBin_CallGraph: NOT YET PROCESSED  : ." <<
      RoseBin_support::HexToString(isSgAsmFunctionDeclaration(next_n->get_SgNode())->get_address()) << " " << next_n << endl;
  }
  // if it has not been seen before, we remember it in visited_f and traverse the call graph

  // traverse the graph from next to node
  std::vector<SgGraphNode*> successors_f;
  std::set<SgGraphNode*> visited_f;

  vector<SgGraphNode*> worklist;
  worklist.push_back(next_n);
  visited_f.insert(next_n);

  while (!worklist.empty()) {
    SgGraphNode* current = worklist.back();
    worklist.pop_back();
    successors_f.clear();
    vizzGraph->getSuccessors(current, successors_f);

    if (debug)
    cerr << "    >>>> RoseBin_CallGraph: successor size of  : ." <<
      RoseBin_support::HexToString(isSgAsmFunctionDeclaration(current->get_SgNode())->get_address()) <<
      " == " << successors_f.size() << endl;
    vector<SgGraphNode*>::iterator succ = successors_f.begin();
    for (;succ!=successors_f.end();++succ) {
      SgGraphNode* next = *succ;
      SgAsmFunctionDeclaration* func_next = isSgAsmFunctionDeclaration(next->get_SgNode());
      ROSE_ASSERT(func_next);

        std::set<SgGraphNode*>::iterator
          it =visited_f.find(next);
        if (it==visited_f.end()) {
          visited_f.insert(next);
          // not has not been visited before, but is it in one of our clusters?
          std::map<SgAsmFunctionDeclaration*,int>::const_iterator clust = visited.find(func_next);
          if (clust!=visited.end()) {
            // found it in clusters
            resultsCluster=clust->second;
    if (debug)
            cerr << "   >>>> RoseBin_CallGraph: node previously iterated : ." <<
              RoseBin_support::HexToString(isSgAsmFunctionDeclaration(func_next)->get_address()) <<
              "  in cluster : " << resultsCluster << endl;
          } else {
            // if it is not in a cluster, remember it for now.
            curr_cluster.insert(next);
            // add to worklist only if this node was not visited on a prior run
            worklist.push_back(next);
            if (debug)
            cerr << "    >>>> RoseBin_CallGraph: iterating currently : ." <<
              RoseBin_support::HexToString(isSgAsmFunctionDeclaration(func_next)->get_address()) <<
              "  cluster found so far is : " << resultsCluster << endl;
          }
        }

    } // for
  } // while

    if (debug)
  cerr << "   >>>> RoseBin_CallGraph: iteration done. " << endl;

  // now we have iterated through all reachable nodes and marked the nodes that have no cluster
  // by putting them into the currentCluster set. If any node has been detected that is
  // part of another cluster, then resulsCluster will be != 0. In that case we need to iterate
  // over all remaining (curr_cluster) nodes and add that resultCluster.
  if (resultsCluster!=0) {

  } else {
    // in this case we increase the global counter and add all nodes we found into the new cluster
    currentCluster++;
    resultsCluster=currentCluster;
  }

  std::set<SgGraphNode*>::const_iterator it = curr_cluster.begin();
  for (;it!=curr_cluster.end();++it) {
    SgGraphNode* node = *it;
    SgAsmFunctionDeclaration* func_next = isSgAsmFunctionDeclaration(node->get_SgNode());
    ROSE_ASSERT(func_next);
    visited[func_next]=resultsCluster;
    if (debug)
    cerr << "    >>>> RoseBin_CallGraph: adding to visited : ." <<
      RoseBin_support::HexToString(isSgAsmFunctionDeclaration(func_next)->get_address()) <<
      "  cluster : " << resultsCluster << "   " << node << endl;
  }

}

void
RoseBin_CallGraphAnalysis::getConnectedComponents(std::map<int,std::set<SgAsmFunctionDeclaration*> >& ret) {
  std::map<SgAsmFunctionDeclaration*,int> visited;

// DQ (4/23/2009): We want the type defined in the base class.
// typedef rose_hash::unordered_map <std::string, SgGraphNode*> nodeType;

  rose_graph_integer_node_hash_map result;
  rose_graph_integer_node_hash_map nodes = vizzGraph->get_node_index_to_node_map();
  rose_graph_integer_node_hash_map::iterator itn2 = nodes.begin();
  int currentCluster=0;
  for (; itn2!=nodes.end();++itn2) {
    //    string hex_address = itn2->first;

    SgGraphNode* node = itn2->second;
    string hex_address = node->get_name();
    //ROSE_ASSERT(hex_address==hex_addr_tmp);

    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    if (func) {
    if (debug)
      std::cerr << "CallGraphAnalysis:: findCluster on function: ." << hex_address << "." <<endl;
      // for each function we need to traverse the callgraph and add the function to a set
      findClusterOfNode(node,currentCluster, visited);
    }
  }

  // convert the visited map into the std::map<int,std::set<SgAsmFunctionDeclaration*> >
  std::map<SgAsmFunctionDeclaration*,int>::const_iterator it = visited.begin();
  for (;it!=visited.end();++it) {
    SgAsmFunctionDeclaration* func = it->first;
    int cluster = it->second;
    std::set<SgAsmFunctionDeclaration*> setFunc;
    std::map<int,std::set<SgAsmFunctionDeclaration*> >::const_iterator findClust = ret.find(cluster);
    if (findClust!=ret.end()) {
      setFunc = findClust->second;
    }
    ROSE_ASSERT(func);
    if (debug)
    cerr << " >> RoseBin_CallGraph: checking function : " << RoseBin_support::HexToString(func->get_address()) << endl;
    setFunc.insert(func);
    ret[cluster]=setFunc;
  }

  // test
  std::map<int,std::set<SgAsmFunctionDeclaration*> >::const_iterator comps = ret.begin();
  for (;comps!=ret.end();++comps) {
      int nr = comps->first;
      if (debug)
      cerr << " CALLGRAPH : found the following component " << nr << endl;
      std::set<SgAsmFunctionDeclaration*>  funcs = comps->second;
      std::set<SgAsmFunctionDeclaration*>::const_iterator it = funcs.begin();
      for (;it!=funcs.end();++it) {
        SgAsmFunctionDeclaration* function = *it;
        string name = function->get_name();
        name.append("_f");
        if (debug)
        cerr << "   CALLGRAPH :  function : " << name << endl;
      }
  }
}

/****************************************************
 * run the compare analysis
 ****************************************************/
void RoseBin_CallGraphAnalysis::run(RoseBin_Graph* vg, string fileN, bool multiedge) {
  vizzGraph = vg;
  fileName = fileN;
  double start=0;
  double ends=0;
  clearMaps();
  //RoseBin_support::setDebugMode(false);
  //RoseBin_support::setDebugModeMin(false);

  func_nr=0;
  //  ROSE_ASSERT(roseBin);
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << "\n ********************** running CallGraphAnalysis ... " << fileName << endl;

  vizzGraph->graph   = new SgIncidenceDirectedGraph(analysisName);
  //SgDirectedGraph(analysisName,analysisName);
  vizzGraph->setGrouping(false);

  ROSE_ASSERT(vizzGraph->graph);
  nr_target_missed=0;
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " running CallGraphAnalysis ... " << endl;
    cout << "\n\n\n -------------------- running CallGraphAnalysis ... " << RoseBin_support::ToString(fileName) << endl;
  }
  start = RoseBin_support::getTime();
  this->traverse(globalBin,preorder);
  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " CFG runtime : " << (double) (ends - start)   << " sec" << endl;
    cerr << " Number of targets missed : " << nr_target_missed << endl;
  }
  //graphs[analysisName] =vizzGraph->graph;
  //  }


    if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " ********************** done running CallGraphAnalysis ... " << endl;
    cerr << " ********************** saving to file ... " << endl;
     }
  // create file
  std::ofstream myfile;
  myfile.open(fileName.c_str());

  string name = "ROSE Graph";
  vizzGraph->printProlog(myfile, name);

  string funcName="";
  start = RoseBin_support::getTime();
  vizzGraph->printNodes(false,this, forward_analysis, myfile,funcName);
  nrNodes=vizzGraph->get_node_index_to_node_map().size();
  //vizzGraph->nodes.clear();

  ROSE_ASSERT(g_algo->info);
  vizzGraph->printEdges(g_algo->info, this,myfile, multiedge);
  nrEdges=vizzGraph->get_node_index_to_edge_multimap_edgesOut().size();
  //  vizzGraph->get_edges()->get_edges().clear();

  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN())
    cerr << " CFG runtime : " << (double) (ends - start)   << " sec" << endl;

  vizzGraph->printEpilog(myfile);
  myfile.close();
}

