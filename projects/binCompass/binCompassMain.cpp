#include <rose.h>
#include <string>

#include "binCompassAnalysisInterface.h"
#include "GraphAnalysisInterface.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <dlfcn.h>

using namespace std;

bool containsArgument(int argc, char** argv, char* pattern) {
for (int i = 2; i < argc ; i++) {
  if (!strcmp(argv[i], pattern)) {
    return true;
  }
 }
  return false;
}

int getdir (string dir, vector<string> &files)
{
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    string name = string(dirp->d_name);
    int find = name.find(".lo");
    if (find>=0) {
      name = name.substr(0,find);
      files.push_back(name);
    }
  }
  closedir(dp);
  return 0;
}

void loadAnalysisFiles(vector <BC_AnalysisInterface*>& checkers, 
		       RoseBin_unparse_visitor* visitor) {  
  string dir = string("analyses");
  vector<string> files = vector<string>();
  getdir(dir,files);
  for (unsigned int i = 0;i < files.size();i++) {
    string name = files[i];
    cout << "Loading Binary Checker --- " << name << endl;

    string filename = "analyses/.libs/lib" + name + ".so";
    void* dl = dlopen((filename).c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!dl) {cerr << "dlopen (" << filename << "): " << dlerror() << endl;}
    ROSE_ASSERT (dl);
    void* symRaw = dlsym(dl, "create");
    if (!symRaw) {cerr << "dlsym (create): " << dlerror() << endl;}
    ROSE_ASSERT (symRaw);
    BC_AnalysisInterface*(*sym)() = (BC_AnalysisInterface*(*)())symRaw;
    BC_AnalysisInterface* intf = sym();
    intf->set_name(name);
    intf->set_unparser(visitor);
    checkers.push_back(intf);
  }
}

void loadGraphAnalysisFiles(vector <BC_GraphAnalysisInterface*>& checkers, 
		       RoseBin_unparse_visitor* visitor) {  
  string dir = string("graphanalyses");
  vector<string> files = vector<string>();
  getdir(dir,files);
  for (unsigned int i = 0;i < files.size();i++) {
    string name = files[i];
    cout << "Loading Binary Graph Checker --- " << name << endl;

    string filename = "graphanalyses/.libs/lib" + name + ".so";
    void* dl = dlopen((filename).c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!dl) {cerr << "dlopen (" << filename << "): " << dlerror() << endl;}
    ROSE_ASSERT (dl);
    void* symRaw = dlsym(dl, "create");
    if (!symRaw) {cerr << "dlsym (create): " << dlerror() << endl;}
    ROSE_ASSERT (symRaw);
    BC_GraphAnalysisInterface*(*sym)() = (BC_GraphAnalysisInterface*(*)())symRaw;
    BC_GraphAnalysisInterface* intf = sym();
    intf->set_name(name);
    intf->set_unparser(visitor);
    checkers.push_back(intf);
  }
}


int main(int argc, char** argv) {

  if (!containsArgument(argc, argv, "-check") && 
      !containsArgument(argc, argv, "-printTree") &&
      !containsArgument(argc, argv, "-callgraph") &&
      !containsArgument(argc, argv, "-cfa") &&
      !containsArgument(argc, argv, "-dfa") 
      ) {argc = 1;}

  if (argc < 2) {
    fprintf(stderr, "Usage: %s executableName [OPTIONS]\n", argv[0]);
    cout << "\nOPTIONS: " <<endl;
    cout << "-check                - run all checkers on binary. " << endl; 
    cout << "-printTree            - create dot file of AST. " << endl; 
    cout << "-callgraph            - perform callgraph analysis and print callgraph.dot file. " << endl; 
    cout << "-cfa                  - perform control flow analysis and print cfg.dot file. " << endl; 
    cout << "-dfa                  - perform dataflow flow analysis and print dfg.dot file. " << endl; 
    cout << "-inter                - perform dataflow analysis interprocedurally (default intraprocedural). " << endl; 
    cout << "-backward             - perform backward analysis (default forward). " << endl; 
    cout << "-gml                  - all graphs (except AST) are saved as gml files (default dot). " << endl; 
    cout << "-mergeedges           - aggregate edges between same nodes. " << endl; 
    cout << "-noedges              - do not print edges into dot or gml file (only nodes). " << endl; 
    return 1;
  }
  string execName = argv[1];

  bool interprocedural = false;
  if (containsArgument(argc, argv, "-inter")) {
    interprocedural = true;
  }
  bool forward = true;
  if (containsArgument(argc, argv, "-backward")) {
    forward = false;
  }
  bool dot = true;
  if (containsArgument(argc, argv, "-gml")) {
    dot = false;
  }
  bool mergedEdges = false;
  if (containsArgument(argc, argv, "-mergeedges")) {
    mergedEdges = true;
  }
  bool edges = true;
  if (containsArgument(argc, argv, "-noedges")) {
    edges = false;
  }

  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  fprintf(stderr, "Starting binCompass frontend...\n");
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  SgAsmFile* file = project->get_file(0).get_binaryFile();

  if (containsArgument(argc, argv, "-printTree")) {
    fprintf(stderr, "Printing AST... _binary_tree.dot\n");
    string filename="_binary_tree.dot";
    AST_BIN_Traversal* trav = new AST_BIN_Traversal();
    trav->run(file->get_global_block(), filename);
  }

  RoseBin_Graph* graph;


  // call graph analysis  *******************************************************
  if (containsArgument(argc, argv, "-callgraph")) {
    cerr << " creating call graph ... " << endl;
    graph= new RoseBin_DotGraph();
     char* callFileName = "callgraph.dot";
    if (dot==false) {
      callFileName = "callgraph.gml";
      graph= new RoseBin_GMLGraph();
    }
    RoseBin_CallGraphAnalysis* callanalysis = new RoseBin_CallGraphAnalysis(file->get_global_block(), NULL);
      callanalysis->run(graph, callFileName, !mergedEdges);
  }

  // control flow analysis  *******************************************************
  if (containsArgument(argc, argv, "-cfa")) {
    char* cfgFileName = "cfg.dot";
   graph= new RoseBin_DotGraph();
   if (dot==false) {
      cfgFileName = "cfg.gml";
      graph= new RoseBin_GMLGraph();
   }
    RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(file->get_global_block(), forward, NULL, edges);
    cfganalysis->run(graph, cfgFileName, mergedEdges);
  }

  if (containsArgument(argc, argv, "-dfa")) {
    cerr << " creating dataflow graph ... " << endl;
    string dfgFileName = "dfg.dot";
    graph= new RoseBin_DotGraph();
    if (dot==false) {
      dfgFileName = "dfg.gml";
      graph= new RoseBin_GMLGraph();
    }
    RoseBin_DataFlowAnalysis* dfanalysis = new RoseBin_DataFlowAnalysis(file->get_global_block(), forward, NULL);
    dfanalysis->init(interprocedural, edges);
    dfanalysis->run(graph, dfgFileName, mergedEdges);
  }

  RoseBin_unparse up;
  up.init(file->get_global_block(), "unparsed.s");
  up.unparse();
  RoseBin_unparse_visitor* visitor = up.getVisitor();
  ROSE_ASSERT(visitor);

  if (containsArgument(argc, argv, "-check")) {
  // get a list of all checkers and traverse
  vector <BC_AnalysisInterface*> checkers;
  vector <BC_GraphAnalysisInterface*> graph_checkers;

  loadAnalysisFiles(checkers, visitor);
  loadGraphAnalysisFiles(graph_checkers, visitor);

  vector <BC_AnalysisInterface*>::const_iterator it = checkers.begin();
  for (;it!=checkers.end();it++) {
    BC_AnalysisInterface* asmf = *it;
    cout << "\nRunning Binary Checker --- " << asmf->get_name() << endl;
    asmf->traverse(file->get_global_block(), preorder);
  }  

  cout << "\n ---------------- preparing to run DataFlowAnalysis " << endl;
  string dfgFileName = "dfg.dot";
  graph= new RoseBin_DotGraph();
  if (dot==false) {
    dfgFileName = "dfg.gml";
    graph= new RoseBin_GMLGraph();
  }
  //  graph->graph   = new SgDirectedGraph(dfgFileName,dfgFileName);
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(file->get_global_block(), forward, NULL, edges);
  cfganalysis->run(graph, dfgFileName, mergedEdges);
  cout << "Graph : " << graph->nodes.size() << endl;

  RoseBin_DataFlowAnalysis* dfanalysis = new RoseBin_DataFlowAnalysis(file->get_global_block(), forward, NULL);
  dfanalysis->init(interprocedural, edges,graph);
  vector<SgDirectedGraphNode*> rootNodes;
  dfanalysis->getRootNodes(rootNodes);
  vector <BC_GraphAnalysisInterface*>::const_iterator it2 = graph_checkers.begin();
  cout << "\n ---------------- running graph checkers : " << graph_checkers.size() << 
    "   rootNodes size : " << rootNodes.size() << endl;
  cout << "Graph : " << graph->nodes.size() << endl;
  for (;it2!=graph_checkers.end();it2++) {
    BC_GraphAnalysisInterface* asmf = *it2;
    ROSE_ASSERT(asmf);
    cout << "\nRunning Binary Graph Checker --- " << asmf->get_name() << "    " <<  endl;
    dfanalysis->traverseGraph(rootNodes, asmf, interprocedural);
  }  
    }  

  return 0;
}


