// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "wholeAST_API.h"
#include "AstDOTGeneration.h"
#include "Diagnostics.h"
#include "StringUtility.h"
// #include "copyGraph.h"
// #include "astGraph.h"
// #include <fstream>

// This file contains support for the visualization of an AST divided into two sets of nodes.
// It is used to visualize the results of the AST Copy mechanism and to visualize the results
// of outlining in the special case where the outlined function (and supporting declarations)
// are moved to a separate file.

// This visualization presents the AST as two sets (using the DOT subgraph mechanism).
// Connections between the subgraphs represent shared IR nodes.  In a properly
// separate AST, the edges connectiong the subgraphs will only be for types (yellow).


using namespace std;
using namespace Rose::Diagnostics;

class GetAllNodesVisitor: public ROSE_VisitTraversal 
   {
  // This traversal is only used in getAllNodes() function (below).
     public:
          set<SgNode*> nodes;
          GetAllNodesVisitor() {}
          virtual void visit(SgNode* n) {nodes.insert(n);}
          virtual ~GetAllNodesVisitor() {}
   };

set<SgNode*>
getAllNodes()
   {
  // This collects all the IR nodes in the memory pool 
  // (so it includes even disconnected IR nodes if they exist).

     GetAllNodesVisitor vis;
     vis.traverseMemoryPool();

     return vis.nodes;
   }


set<SgNode*>
getAllNodes( SgNode* node )
   {
  // This collects all the IR nodes in the specified subtree.

     class GetAllNodesVisitor: public SgSimpleProcessing
        {
       // This traversal is only used in getAllNodes() function (below).
          public:
               set<SgNode*> nodes;
               GetAllNodesVisitor() {}
               virtual void visit(SgNode* n) {nodes.insert(n);}
               virtual ~GetAllNodesVisitor() {}
        };

     GetAllNodesVisitor visitor;
     visitor.traverse(node,preorder);

     return visitor.nodes;
   }


void graphNodesAfterCopy(const set<SgNode*>& oldNodes, string filename)
   {
  // This function was written by Jeremiah.

  // This is a hack, but it works for now
  // cerr << "Do not run this code in an untrusted directory -- it makes temporary files with constant names" << endl;
     string temp_filename      = filename + "_temp";
     string old_nodes_filename = filename + "_old_nodes";
  // generateWholeGraphOfAST("temp");
     generateWholeGraphOfAST(temp_filename.c_str());
        {
       // ofstream fs("old_nodes");
          ofstream fs(old_nodes_filename.c_str());
          for (set<SgNode*>::const_iterator i = oldNodes.begin(); i != oldNodes.end(); ++i)
             {
               if (i != oldNodes.begin()) fs << ' ';
                    fs << (uintptr_t)(*i);
             }

          fs << endl;
        }

        std::string cmd = "tclsh " + std::string(ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR) +
                          "/tests/nonsmoke/functional/CompileTests/copyAST_tests/make_copy_graph.tcl " +
                          old_nodes_filename + " " + temp_filename + ".dot " + filename + ".dot";
        if (system(cmd.c_str()))
            mlog[ERROR] <<"command failed: \"" + Rose::StringUtility::cEscape(cmd) <<"\"\n";
  // system("rm temp.dot");
  // system("rm old_nodes");
   }
