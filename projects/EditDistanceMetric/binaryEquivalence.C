// This code test two binary executable files for equivalence and
// reports the details of what is different when difference are 
// detected in either the binary executable file format or the 
// instruction stream.

// Traverse and add lables for the order of the visits to each node then
// build the vectors as pairs of elements (the node enum value and the 
// the integer order of visit).

#include <rose.h>

using namespace std;

#include <labelTrace.h>
#include <treeIsomorphism.h>
#include <treeDiff.h>

// This version of the code is not used, using astDiff.C instead.
#error "DEAD CODE!"

int
main(int argc, char** argv)
   {
  // ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST equivalence test (main): time (sec) = ",true);

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Just set the project, the report will be generated upon calling the destructor for "timer"
     timer.set_project(project);

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
     sequenceAlignment ( traversalTraceList );
     printf ("Done with call to sequenceAlignment() \n");
#endif

     outputAST(project);

  // Unparse the binary executable (as a binary, as an assembler text file, 
  // and as a dump of the binary executable file format details (sections)).
     return backend(project);
   }
