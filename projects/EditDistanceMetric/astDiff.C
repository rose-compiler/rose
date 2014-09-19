// This code test two binary executable files for equivalence and
// reports the details of what is different when difference are 
// detected in either the binary executable file format or the 
// instruction stream.

// Traverse and add lables for the order of the visits to each node then
// build the vectors as pairs of elements (the node enum value and the 
// the integer order of visit).

#include <rose.h>
#include <EditDistance/TreeEditDistance.h>
#include <sawyer/CommandLine.h>
#include <sawyer/Stopwatch.h>

using namespace std;
using namespace rose;

// #include "labelTrace.h"
#include "treeIsomorphism.h"
#include "treeDiff.h"
#include "sequenceGeneration.h"
#include "treeEditDistance.h"
#include "commandLine.h"

int
main(int argc, char** argv)
   {
     ios::sync_with_stdio();                            // Syncs C++ and C I/O subsystems!
     rose::Diagnostics::initialize();                   // because librose doesn't initialize itself until frontend()

     Settings settings;
     std::vector<std::string> roseArgs = parseCommandLine(argc, argv, settings).unreachedArgs();
     roseArgs.insert(roseArgs.begin(), std::string(argv[0])); // not sure why frontend needs this, but it does.
     SgProject* project = frontend(roseArgs);
     ROSE_ASSERT (project != NULL);

  // Internal AST consistancy tests.
     AstTests::runAllTests(project);

     ROSE_ASSERT(project->numberOfFiles() == 2);

     int numberOfNodesInFile_0 = project->get_fileList()[0]->numberOfNodesInSubtree();
     int numberOfNodesInFile_1 = project->get_fileList()[1]->numberOfNodesInSubtree();

     printf ("numberOfNodesInFile_0 = %d \n",numberOfNodesInFile_0);
     printf ("numberOfNodesInFile_1 = %d \n",numberOfNodesInFile_1);

#if 0
     diff(project->get_fileList()[0],project->get_fileList()[1]);
#else

     vector< vector< SgNode* > > traversalTraceList = generateTraversalTraceList (project->get_fileList()[0],project->get_fileList()[1], /* minDepth = 0 */ 0, /* infinite maxDepth */ -1 );

     printf ("Calling sequenceAlignment()... traversalTraceList.size() = %zu \n",traversalTraceList.size());
     sequenceAlignment  ( traversalTraceList );
     printf ("Done with call to sequenceAlignment() \n");
#endif

     // This method simply prints the edits as a side effect and doesn't return anything useful.
     Sawyer::Stopwatch tedLocalTimer;
     tree_edit_distance (project->get_fileList()[0],project->get_fileList()[1]);
     std::cout <<"local implementation of tree edit distance took " <<tedLocalTimer <<" seconds\n";

     // This stuff uses the new edit distance analysis in librose
     std::cout <<"Using TreeEditDistance from librose:\n";
     TreeEditDistance analysis;
     Sawyer::Stopwatch tedRoseTimer;
     analysis.compute(project->get_fileList()[0], project->get_fileList()[1],  // the subtrees to compare
                      project->get_fileList()[0], project->get_fileList()[1]); // their file restrictions
     std::cout <<"librose implementation of tree edit distance took " <<tedRoseTimer <<" seconds\n";
     TreeEditDistance::Edits edits = analysis.edits();
     std::cout <<"  Nodes in source tree: " <<analysis.sourceTreeNodes().size() <<"\n"
               <<"  Nodes in target tree: " <<analysis.targetTreeNodes().size() <<"\n"
               <<"  Number of edits:      " <<edits.size() <<"\n"
               <<"  Total cost of edits:  " <<analysis.cost() <<"\n"
               <<"  Relative cost:        " <<analysis.relativeCost() <<"\n"
               <<"  Individual edits:\n";
     BOOST_FOREACH (const TreeEditDistance::Edit &edit, edits)
         std::cout <<"    " <<edit <<"\n";
     std::ofstream graphViz("output-rose.dot");
     analysis.emitGraphViz(graphViz);
     std::cout <<"librose implementation of tree edit distance + output took " <<tedRoseTimer <<" seconds\n";
   }
