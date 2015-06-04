// This code test two binary executable files for equivalence and
// reports the details of what is different when difference are 
// detected in either the binary executable file format or the 
// instruction stream.

// Traverse and add lables for the order of the visits to each node then
// build the vectors as pairs of elements (the node enum value and the 
// the integer order of visit).

#include <rose.h>
#include <EditDistance/TreeEditDistance.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stopwatch.h>

using namespace std;
using namespace rose;

// #include "labelTrace.h"
#include "treeIsomorphism.h"
#include "treeDiff.h"
#include "sequenceGeneration.h"
#include "treeEditDistance.h"
#include "commandLine.h"

// Parses only those switches that this tool recognizes and skips all others, leaving them for ROSE's frontend(). The tool
// switches come first and the ROSE switches are after a "--" separator switch.  We wouldn't need to do it this way, but three
// things conspire to make this the best approach: (1) frontend() doesn't throw an error when it doesn't recognize a switch,
// (2) ROSE doesn't provide a description of its command-line, and (3) ROSE doesn't provide a mechanism to check the validity
// of a command line without actually applying it.
Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup gen = CommandlineProcessing::genericSwitches();
    SwitchGroup tool = toolCommandLineSwitches(settings);

    Parser parser;
    parser
        .purpose("demonstrates tree edit distance")
        .doc("synopsis",
             "@prop{programName} [@v{tool_switches}] -- [@v{rose_switches}] @v{specimen1} @v{specimen2}")
        .doc("description",
             "This tool performs tree edit distance between two source files a couple different ways in order to compare "
             "the old, local implementation and the new librose implementation. See @s{use-old} for more details.  The "
             "output of the old implementation and the new implementation are not usually identical because there are "
             "usually multiple edit paths with the same total cost.");

    return parser.with(gen).with(tool).parse(argc, argv).apply();
}

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

     if (settings.useOldImplementation) {
#if 0
         diff(project->get_fileList()[0],project->get_fileList()[1]);
#else

         vector< vector< SgNode* > > traversalTraceList = generateTraversalTraceList (project->get_fileList()[0],
                                                                                      project->get_fileList()[1],
                                                                                      /* minDepth = 0 */ 0,
                                                                                      /* infinite maxDepth */ -1 );
         printf ("Calling sequenceAlignment()... traversalTraceList.size() = %zu \n",traversalTraceList.size());
         sequenceAlignment  ( traversalTraceList );
         printf ("Done with call to sequenceAlignment() \n");
#endif

         // This method simply prints the edits as a side effect and doesn't return anything useful.
         Sawyer::Stopwatch tedLocalTimer;
         tree_edit_distance (project->get_fileList()[0],project->get_fileList()[1]);
         std::cout <<"local implementation of tree edit distance took " <<tedLocalTimer <<" seconds\n";
     }

     // This stuff uses the new edit distance analysis in librose
     std::cout <<"Using TreeEditDistance from librose:\n";
     EditDistance::TreeEditDistance::Analysis analysis;
     Sawyer::Stopwatch tedRoseTimer;
     analysis.compute(project->get_fileList()[0], project->get_fileList()[1],  // the subtrees to compare
                      project->get_fileList()[0], project->get_fileList()[1]); // their file restrictions
     std::cout <<"librose implementation of tree edit distance took " <<tedRoseTimer <<" seconds\n";
     EditDistance::TreeEditDistance::Edits edits = analysis.edits();
     std::cout <<"  Nodes in source tree: " <<analysis.sourceTreeNodes().size() <<"\n"
               <<"  Nodes in target tree: " <<analysis.targetTreeNodes().size() <<"\n"
               <<"  Number of edits:      " <<edits.size() <<"\n"
               <<"  Total cost of edits:  " <<analysis.cost() <<"\n"
               <<"  Relative cost:        " <<analysis.relativeCost() <<"\n"
               <<"  Individual edits:\n";
     BOOST_FOREACH (const EditDistance::TreeEditDistance::Edit &edit, edits)
         std::cout <<"    " <<edit <<"\n";
     std::ofstream graphViz("output-rose.dot");
     analysis.emitGraphViz(graphViz);
     std::cout <<"librose implementation of tree edit distance + output took " <<tedRoseTimer <<" seconds\n";
   }
