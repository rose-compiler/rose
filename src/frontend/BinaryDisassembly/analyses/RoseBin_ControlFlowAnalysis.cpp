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
}

