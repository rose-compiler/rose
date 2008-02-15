/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 17Oct07
 * Description: Creates the binary call graph
 ****************************************************/

#include "rose.h"
#include "RoseBin_CallGraphAnalysis.h"

using namespace std;


/****************************************************
 * run the compare analysis
 ****************************************************/
void RoseBin_CallGraphAnalysis::run(RoseBin_Graph* vg, string fileN, bool multiedge) {
  vizzGraph = vg;
  fileName = fileN;
  double start=0;
  double ends=0;

  func_nr=0;
  //  ROSE_ASSERT(roseBin);
  if (RoseBin_support::DEBUG_MODE_MIN()) 
    cerr << "\n ********************** running CallGraphAnalysis ... " << fileName << endl;

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
  vizzGraph->setGrouping(false);

  nr_target_missed=0;
  if (RoseBin_support::DEBUG_MODE_MIN()) 
    cerr << " running CallGraphAnalysis ... " << endl;
  cout << "\n\n\n -------------------- running CallGraphAnalysis ... " << RoseBin_support::ToString(fileName) << endl;
  start = RoseBin_support::getTime();
  this->traverse(globalBin,preorder);
  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN()) {
    cerr << " CFG runtime : " << (double) (ends - start)   << " sec" << endl;
    cerr << " Number of targets missed : " << nr_target_missed << endl;
  }
  //graphs[analysisName] =vizzGraph->graph;
  //  }

  //graph->set_nodes(nodes);
  //graph->set_edges(edges);
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
  vizzGraph->printNodes(false,this, myfile,funcName);
  vizzGraph->nodes.clear();

  vizzGraph->printEdges(this,myfile, multiedge);
  vizzGraph->edges.clear();

  ends = RoseBin_support::getTime();
  if (RoseBin_support::DEBUG_MODE_MIN()) 
    cerr << " CFG runtime : " << (double) (ends - start)   << " sec" << endl;

  vizzGraph->printEpilog(myfile);
  myfile.close();  
}

