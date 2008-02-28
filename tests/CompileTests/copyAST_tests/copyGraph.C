#include "copyGraph.h"
#include "astGraph.h"
#include <fstream>

using namespace std;

class GetAllNodesVisitor: public ROSE_VisitTraversal {
  public:
  set<SgNode*> nodes;
  GetAllNodesVisitor() {}
  virtual void visit(SgNode* n) {nodes.insert(n);}
  virtual ~GetAllNodesVisitor() {}
};

set<SgNode*> getAllNodes() {
  GetAllNodesVisitor vis;
  vis.traverseMemoryPool();
  return vis.nodes;
}

void graphNodesAfterCopy(const set<SgNode*>& oldNodes, string filename) {
  // This is a hack, but it works for now
  // cerr << "Do not run this code in an untrusted directory -- it makes temporary files with constant names" << endl;
  string temp_filename      = filename + "_temp";
  string old_nodes_filename = filename + "_old_nodes";
// generateWholeGraphOfAST("temp");
  generateWholeGraphOfAST(temp_filename.c_str());
  {
 // ofstream fs("old_nodes");
    ofstream fs(old_nodes_filename.c_str());
    for (set<SgNode*>::const_iterator i = oldNodes.begin(); i != oldNodes.end(); ++i) {
      if (i != oldNodes.begin()) fs << ' ';
      fs << (uintptr_t)(*i);
    }
    fs << endl;
  }
  system(("tclsh " ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR "/tests/CompileTests/copyAST_tests/make_copy_graph.tcl " + old_nodes_filename + " " + temp_filename + ".dot " + filename + ".dot").c_str());
  // system("rm temp.dot");
  // system("rm old_nodes");
}
