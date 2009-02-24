
// This implements the support for merging ASTs. It should merge perfectly
// if we handled the merging of everthing, but namespaces and some function declarations
// will be a problem because they will generate unique names that are not really unique.
//    1) Namespaces:
//       the same namespace will generate the same name for each instance of it in
//       the source program (remember that namespaces are re-entrant).  Thus we can't
//       namespaces unless we verify same position in source.
//    2) Function declarations:
//       function prototypes can be repeated and will all generate the same unique name
//       independent of if they declare parameters with default arguments.  Thus we can't
//       merge function declarations unless we verify same position in source.
//    3) Some declarations return an empty name (we might want to fix that) but these are
//       clearly not unique enough since that makes "class { int x; };" has no name.
//    4) variable declarations that use "extern" are mangled the same. e.g 
//       "int x;" and "extern int x;"

#include <rose.h>

#include "merge.h"

#include "colorTraversal.h"

#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH 2000
#define DISPLAY_INTERNAL_DATA 0

using namespace std;

// Global variable that functions can use to make sure that there IR nodes were not deleted!
// This is used for debugging only (tests in assertions).
set<SgNode*> finalDeleteSet;

// Supporting function to process the commandline
void commandLineProcessing (int & argc, char** & argv, bool & skipFrontendSpecificIRnodes)
   {
     list<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("Preprocessor (before): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // bool skipFrontendSpecificIRnodes = false;
  // Add a test for a custom command line option (and remove the options from the commandline; by passing true as last parameter)
     int integerOptionForSupressFrontendCode = 0;
     if ( CommandlineProcessing::isOptionWithParameter(argc,argv,"-merge:","(s|suppress_frontend_code)",integerOptionForSupressFrontendCode,true) )
        {
          printf ("Turning on AST merge suppression of graphing fronend-specific IR nodes (set to %d) \n",integerOptionForSupressFrontendCode);
          skipFrontendSpecificIRnodes = true;
        }

  // Adding a new command line parameter (for mechanisms in ROSE that take command lines)

     printf ("argc = %d \n",argc);
     l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("l.size() = %ld \n",l.size());
     printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(l).c_str());
   }



int
main ( int argc, char** argv )
   {
  // ****************************************************************************
  // **************************  Command line Processing  ***********************
  // ****************************************************************************
     bool skipFrontendSpecificIRnodes = false;
     commandLineProcessing(argc,argv,skipFrontendSpecificIRnodes);
  // ****************************************************************************

  // ****************************************************************************
  // **************************      Build the AST    ***************************
  // ****************************************************************************
     SgProject* project = frontend (argc, argv);
     ROSE_ASSERT(project != NULL);

  // Generate the filename to be used as a base for generated dot files (graphs)
     string filename = SageInterface::generateProjectName(project);

     printf ("\n\n");
     printf ("**************************************************************** \n");
     printf ("**********************  START AST MERGE ************************ \n");
     printf ("**************************************************************** \n");

     int numberOfASTnodesBeforeMerge = numberOfNodes();
     printf ("numberOfASTnodesBeforeMerge = %d MAX_NUMBER_OF_IR_NODES_TO_GRAPH = %d \n",numberOfASTnodesBeforeMerge,MAX_NUMBER_OF_IR_NODES_TO_GRAPH);
     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH)
        {
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMerge");
          set<SgNode*> emptySet;
          SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMergeWholeAST_unfiltered",emptySet);
          set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
          SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMergeWholeAST",skippedNodeSet);
          SimpleColorFilesTraversal::generateGraph(project,filename+"_beforeMerge",skippedNodeSet);
        }
#if 0
     printf ("Exiting after building the pre-merged AST \n");
     ROSE_ASSERT(false);
#endif

#if 1
  // Run AST tests (takes a while on large ASTs, so we sometime skip this for some phases of development on AST merge)
     printf ("Running AST tests \n");
     AstTests::runAllTests(project);
     printf ("Running AST tests: DONE \n");
#endif

  // ****************************************************************************
  // **************************   Mangled Name Tests   **************************
  // ****************************************************************************
  // Test the generateUniqueName() function (everywhere in the AST, so that we will know that we can rely on it!)
     printf ("Running testUniqueNameGenerationTraversal (more AST tests) \n");
     testUniqueNameGenerationTraversal();
     printf ("Running testUniqueNameGenerationTraversal (more AST tests): DONE \n");

  // TestParentPointersOfSymbols::test();

  // Present the STL types more directly in the source code.
  // ReplacementMapTraversal::ODR_ViolationType ODR_Violations;
     list< pair<SgNode*,SgNode*> > ODR_Violations;

#if 0
  // This is a destructive test on the AST to test the deleteOrphanIRnodesInMemoryPool()
  // function (which should remove all entries). We don't use this capablity because
  // we try to be more precise and if so we should not have orphan IR nodes.
     printf ("Generate the graph before the NullTree \n");
     SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeNullTree",intermediateDeleteSet);
     NullTree();
     printf ("Generate the graph after the NullTree \n");
     SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterNullTree",intermediateDeleteSet);
     ROSE_ASSERT(false);
#endif

  // Remove any simple unattached AST fragments not pointed to by other IR nodes
  // deleteOrphanIRnodesInMemoryPool();

  // TestParentPointersOfSymbols::test();

     int replacementHashTableSize = 1001;
     int mangledNameHashTableSize = 1001;

  // ****************************************************************************
  // ***********************  Generate Mangled Name Map   ***********************
  // ****************************************************************************
  // This traverses the memory pool (so it sees everything) and build mangled names
  // for anything that is judged to be sharable (see implementation for what is shared).
     printf ("\n\n************************************************************\n");
  // MangledNameMapTraversal::SetOfNodesType intermediateDeleteSet;
     set<SgNode*>  intermediateDeleteSet;
     MangledNameMapTraversal::MangledNameMapType mangledNameMap (mangledNameHashTableSize);

     printf ("Calling getMangledNameMap() \n");
     generateMangledNameMap(mangledNameMap,intermediateDeleteSet);
     printf ("Calling getMangledNameMap(): DONE \n");
     printf ("************************************************************\n\n");

     printf ("mangledNameMap.size() = %ld intermediateDeleteSet = %ld \n",mangledNameMap.size(),intermediateDeleteSet.size());
#if DISPLAY_INTERNAL_DATA || 0
     MangledNameMapTraversal::displayMagledNameMap(mangledNameMap);
#endif
#if DISPLAY_INTERNAL_DATA > 1 || 0
     displaySet(intermediateDeleteSet,"Delete set computed by getMangledNameMap");
#endif

#if 0
     printf ("Existing after creation of mangle name IR node map \n");
     ROSE_ASSERT(false);
#endif

     int numberOfASTnodesBeforeCopy = numberOfNodes();
     printf ("Before AST copy: numberOfASTnodesBeforeCopy = %d intermediateDeleteSet = %zu \n",numberOfASTnodesBeforeCopy,intermediateDeleteSet.size());

#if 0
  // printf ("Generate the graph before the copy \n");
  // SimpleColorMemoryPoolTraversal::generateGraph("beforeMergeWholeAST",intermediateDeleteSet);

  // Make a copy of the whole AST (to test the merge mechanism)
  // This actually does not work well because of the numbeer of 
  // things in the AST copy that are references to the original AST.
  // So we can't expect the same level of efficiency of the merge 
  // and it is harder to debug and evaluate the results.  However,
  // once the AST merge is working better it should also work on 
  // the a copy make using the AST copy mechanism.
     printf ("Calling the AST copy mechanism \n");
     SgFile* existingFile = &(project->get_file(0));
     ROSE_ASSERT(existingFile != NULL);

     setOfIRnodes = generateNodeListFromAST (project);
     setOfIRnodes = generateNodeListFromMemoryPool();

     SgFile* newFile = isSgFile(copyAST(existingFile));
     ROSE_ASSERT(newFile != NULL);
     project->set_file(*newFile);
#endif

#if 0
  // Make a copy of the whole AST (to test the merge mechanism)
     printf ("Calling the AST copy mechanism \n");
     SgGlobal* existingGlobalScope = project->get_file(0).get_globalScope();
     ROSE_ASSERT(existingGlobalScope != NULL);
     SgGlobal* newGlobalScope = dynamic_cast<SgGlobal*>(copyAST(existingGlobalScope));
     ROSE_ASSERT(newGlobalScope != NULL);

  // DQ (4/1/2006): Can't call copy mechanism from SgProject (need to fix this).
  // SgProject* newProject = dynamic_cast<SgProject*>(copyAST(project));
  // ROSE_ASSERT(newProject != NULL);
#endif

  // DQ (1/20/2007): Commented out since at least one SgClassType should not have been deleted!
  // printf ("Commented out call to deleteOrphanIRnodesInMemoryPool() since at least one SgClassType should not have been deleted! \n");
  // Remove any simple unattached AST fragments not pointed to by other IR nodes
  // deleteOrphanIRnodesInMemoryPool();

  // If this is not too large of a graph then build the DOT output file for visualization.
  // This is an expensive task fro large ASTs, and the layout for the DOT graph is 
  // exponential so not practical.
     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH)
        {
          printf ("Generate the graph after the copy intermediateDeleteSet = %ld \n",intermediateDeleteSet.size());
          SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMangledNameMapWholeAST",intermediateDeleteSet);
          SimpleColorFilesTraversal::generateGraph(project,filename+"_afterMangledNameMap");
        }

#if 0
     printf ("Exiting as part of test ... \n");
     exit(1);
#endif

     int numberOfASTnodesAfterCopy = numberOfNodes();
     printf ("After AST copy: numberOfASTnodesAfterCopy = %d (%d increase) intermediateDeleteSet = %ld \n",
          numberOfASTnodesAfterCopy,numberOfASTnodesAfterCopy-numberOfASTnodesBeforeCopy,intermediateDeleteSet.size());

  // DQ (2/19/2007): Build the replacement map externally and pass it in to avoid copying.
     ReplacementMapTraversal::ReplacementMapType replacementMap(replacementHashTableSize);

  // ****************************************************************************
  // ***********************   Generate Replacement Map   ***********************
  // ****************************************************************************
  // Build the replacement Map, locations (pointers to pointers) in the AST where updates will be done (during the fixupTraversal).
  // Their is not side-effect to the AST from this traversal (the memory pool is used so that ALL IR nodes will be traversed).
     printf ("\n\n************************************************************\n");
     printf ("Calling replacementMapTraversal() \n");
  // ReplacementMapTraversal::ReplacementMapType replacementMap = replacementMapTraversal(mangledNameMap,ODR_Violations,intermediateDeleteSet);
     replacementMapTraversal(mangledNameMap,replacementMap,ODR_Violations,intermediateDeleteSet);
     printf ("Calling replacementMapTraversal(): DONE \n");
     printf ("************************************************************\n\n");
#if DISPLAY_INTERNAL_DATA || 0
     printf ("\n\n After replacementMapTraversal(): replacementMap: \n");
     ReplacementMapTraversal::displayReplacementMap(replacementMap);

     printf ("\n\n After replacementMapTraversal(): intermediateDeleteSet: \n");
     displaySet(intermediateDeleteSet,"After replacementMapTraversal");
#endif

  // TestParentPointersOfSymbols::test();

     printf ("After replacementMapTraversal: replacementMap = %ld intermediateDeleteSet = %ld \n",replacementMap.size(),intermediateDeleteSet.size());

#if 0
     printf ("Exiting as part of test after computing the replacementMap ... \n");
     exit(1);
#endif

     printf ("Calling ReplacementMapTraversal::buildListOfODRviolations() \n");
     set<SgNode*> problemSubtreeSet = ReplacementMapTraversal::buildListOfODRviolations(ODR_Violations);
     printf ("Calling ReplacementMapTraversal::buildListOfODRviolations(): DONE \n");

     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH)
        {
          printf ("Generate the graph after computing the replacement map (ODR_Violations.size() = %ld) \n",ODR_Violations.size());
          SimpleColorFilesTraversal::generateGraph(project,filename+"_ODR_violations_afterReplacementMap",problemSubtreeSet);

          set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
          SimpleColorFilesTraversal::generateGraph(project,filename+"afterReplacementMap",skippedNodeSet);
        }

#if 0
     printf ("Exiting as part of test after computing the problemSubtreeSet ... \n");
     exit(1);
#endif

  // ****************************************************************************
  // ***********************  Fixup AST to Share IR nodes  **********************
  // ****************************************************************************
  // This traversal of the replacement map modified the AST to reset pointers to subtrees that will be shared.
  // The whole AST is traversed (using the memory pool traversal) and the data member pointers to IR nodes that
  // are found in the replacement map are used to lookup the replacement values that are used to reset the 
  // pointers in the AST. As the replacement is computed the pointer values that are marked in the replacement
  // list for update are added to the intermediateDeleteSet.
     printf ("\n\n************************************************************\n");
     printf ("Calling fixupTraversal() \n");
     fixupTraversal(replacementMap,intermediateDeleteSet);
     printf ("Calling fixupTraversal(): DONE \n");
     printf ("************************************************************\n\n");
#if DISPLAY_INTERNAL_DATA > 1 || 0
     printf ("\n\n After replacementMapTraversal(): intermediateDeleteSet: \n");
     displaySet(intermediateDeleteSet,"After fixupTraversal");
#endif

#if 0
     printf ("Exiting after fixupTraversal ... \n");
     exit(1);
#endif

  // TestParentPointersOfSymbols::test();

     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH)
        {
          printf ("Generate the graph after the merge intermediateDeleteSet = %ld \n",intermediateDeleteSet.size());
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST",intermediateDeleteSet);
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST",getSetOfFrontendSpecificNodes(requiredNodesSet));
          set<SgNode*> emptySet;
          SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST_unfiltered",emptySet);
          SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST",getSetOfFrontendSpecificNodes());
          SimpleColorFilesTraversal::generateGraph(project,filename+"_afterMerge");
        }

     int numberOfASTnodesBeforeDelete = numberOfNodes();
     printf ("Before AST delete: numberOfASTnodesBeforeDelete = %d intermediateDeleteSet = %zu finalDeleteSet = %zu \n",numberOfASTnodesBeforeDelete,intermediateDeleteSet.size(),finalDeleteSet.size());

  // ****************************************************************************
  // *****************  Compute Final Set of IR Nodes To Delete  ****************
  // ****************************************************************************
// Using the intermediateDeleteSet and the requiredNodesSet
     set<SgNode*> requiredNodesSet = buildRequiredNodeList(project);
#if DISPLAY_INTERNAL_DATA > 1 || 0
     displaySet (requiredNodesSet,"After fixupTraversal(): requiredNodesSet");
#endif

  // displayDeleteSet (intermediateDeleteSet);

     printf ("requiredNodesSet.size() = %ld \n",requiredNodesSet.size());

#if 0
     printf ("Exiting after computing required IR nodes ... \n");
     exit(1);
#endif

     printf ("Calling computeSetDifference() \n");
  // Computer the set difference between the nodes to delete and the nodes that are required!
  // set<SgNode*> finalDeleteSet = computeSetDifference(intermediateDeleteSet,requiredNodesSet);
     finalDeleteSet = computeSetDifference(intermediateDeleteSet,requiredNodesSet);
     printf ("Calling compute set difference(): DONE \n");

     printf ("Output the set difference size = %ld \n",finalDeleteSet.size());

#if DISPLAY_INTERNAL_DATA || 0
     displaySet(finalDeleteSet,"finalDeleteSet");
#endif

  // DQ (2/15/2007): Error checking on the finalDeleteSet
  // deleteSetErrorCheck( project, requiredNodesSet );
     deleteSetErrorCheck( project, finalDeleteSet );

     printf ("Calling computeSetIntersection() \n");
     set<SgNode*> intersectionSet = computeSetIntersection(intermediateDeleteSet,requiredNodesSet);
     printf ("Calling computeSetIntersection(): DONE \n");

     printf ("intersectionSet.size() = %ld \n",intersectionSet.size());
#if DISPLAY_INTERNAL_DATA > 1 || 0
     displaySet(intersectionSet,"intersectionSet");
#endif

#if 0
  // DQ (2/19/2007): We can't pass the mangled name tests because we have IR nodes that are not linked to properly 
  // after the merge (those IR nodes that ate on the delete list for example). 
  // Run AST tests (takes a while on large ASTs, so we sometime skip this for some phases of development on AST merge)
     printf ("Running AST tests (after merge and before delete) \n");
     AstTests::runAllTests(project);
     printf ("Running AST tests (after merge and before delete): DONE \n");
#endif


  // Print the global symbol table
  // project->get_file(0).get_globalScope()->get_symbol_table()->print("Global Scope");

  // ****************************************************************************
  // ************  Delete AST IR Node Made Redundant Due To Sharing  ************
  // ****************************************************************************
     printf ("\n\n************************************************************\n");
     printf ("Calling deleteNodes() \n");
     deleteNodes(finalDeleteSet);
  // deleteNodes(intersectionSet);
     printf ("Calling deleteNodes(): DONE \n");
     printf ("************************************************************\n\n");

#if 0
  // Remove any simple unattached AST fragments not pointed to by other IR nodes
     printf ("Calling deleteOrphanIRnodesInMemoryPool() \n");
     deleteOrphanIRnodesInMemoryPool();
     printf ("Calling deleteOrphanIRnodesInMemoryPool(): DONE \n");
#endif

  // TestParentPointersOfSymbols::test();

  // printf ("Generate the graph after the deleting redundant IR nodes \n");
  // SimpleColorMemoryPoolTraversal::generateGraph("afterDelete",setOfIRnodes);
  // SimpleColorFilesTraversal::generateGraph(project,"afterDelete");
  // SimpleColorMemoryPoolTraversal::generateGraph("afterDelete",setOfIRnodes);
  // SimpleColorFilesTraversal::generateGraph(project,"afterDelete",problemSubtreeSet);

  // Shared nodes are not marked, but they could be.
  // set<SgNode*> sharedNodeSet = getSetOfSharedNodes();

  // *******************************************************************************************
  // ******** Generate Frontend Specific IR Node Set (for visualization of merged AST) *********
  // *******************************************************************************************
  // These IR nodes are required to be in the AST, but we don't want to display them since they make the graphs overly complex.
     set<SgNode*> skippedNodeSet;

  // This option is controled by a commandline parameter "-merge:s" or "-merge:suppress_frontend_code"
     printf ("skipFrontendSpecificIRnodes = %s \n",(skipFrontendSpecificIRnodes == true) ? "true" : "false");
     if (skipFrontendSpecificIRnodes == true)
        {
       // Skip IR nodes from the front-end
          printf ("Calling getSetOfFrontendSpecificNodes() \n");
          skippedNodeSet = getSetOfFrontendSpecificNodes();
        }
  // printf ("sharedNodeSet size = %ld skippedNodeSet size = %ld \n",sharedNodeSet.size(),skippedNodeSet.size());
     printf ("skippedNodeSet size = %ld \n",skippedNodeSet.size());
  // displaySet(skippedNodeSet,"result from getSetOfFrontendSpecificNodes() function");

  // set<SgNode*> skippedNodeSet          = sharedNodeSet;
  // skippedNodeSet.insert(frontendSpecificNodeSet.begin(),frontendSpecificNodeSet.end());
  // set<SgNode*> skippedNodeSet          = frontendSpecificNodeSet;


  // printf ("sharedNodeSet size = %ld frontendSpecificNodeSet size = %ld skippedNodeSet size = %ld \n",sharedNodeSet.size(),frontendSpecificNodeSet.size(),skippedNodeSet.size());

#if 1
  // These tests currently fail!
  // Run AST tests (takes a while on large ASTs, so we sometime skip this for some phases of development on AST merge)
     printf ("Running AST tests (after delete, and before graph generation) \n");
     AstTests::runAllTests(project);
     printf ("Running AST tests (after delete, and before graph generation): DONE \n");
#endif

     int numberOfASTnodesAfterDelete = numberOfNodes();
     if (numberOfASTnodesAfterDelete < MAX_NUMBER_OF_IR_NODES_TO_GRAPH)
        {
       // SimpleColorMemoryPoolTraversal::generateGraph("afterDeleteWholeAST",setOfIRnodes);
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterDeleteWholeAST",sharedNodeSet);
          SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterDeleteWholeAST",skippedNodeSet);
          SimpleColorFilesTraversal::generateGraph(project,filename+"_ODRviolations",problemSubtreeSet);
        }

  // ******************************************************************************
  // ****************** Compute Efficency of AST Merge Mechanism ******************
  // ******************************************************************************
     printf ("numberOfASTnodesBeforeMerge = %d numberOfASTnodesBeforeDelete = %d \n",numberOfASTnodesBeforeMerge,numberOfASTnodesBeforeDelete);
     float percentageDecrease = 100.0 - ( ((float) numberOfASTnodesAfterDelete) / ((float) numberOfASTnodesBeforeDelete) ) * 100.0;
     float numberOfFiles = project->numberOfFiles();
     float mergeEfficency = 0.0;
     if (numberOfFiles == 1)
        {
       // For a single file we expect no decrease to ge 100% efficiency (but we do get something for better sharing of types)
          mergeEfficency = 1.0 + percentageDecrease;
        }
       else
        {
          float fileNumberMultiplier = numberOfFiles / (numberOfFiles - 1);
          mergeEfficency = percentageDecrease * fileNumberMultiplier;
        }

     printf ("\n\n");
     printf ("******************************************************************************************************************************************************************** \n");
     printf ("After AST delete: numberOfASTnodesBeforeMerge = %d numberOfASTnodesAfterDelete = %d (%d node decrease: %2.4f percent decrease, mergeEfficency = %2.4f) \n",
          numberOfASTnodesBeforeMerge,numberOfASTnodesAfterDelete,numberOfASTnodesBeforeDelete-numberOfASTnodesAfterDelete,percentageDecrease,mergeEfficency);
     printf ("******************************************************************************************************************************************************************** \n\n\n");

#if 0
     reportUnsharedDeclarationsTraversal();
#else
     printf ("Note: Skipping reportUnsharedDeclarationsTraversal(): generates a list of un-shared constructs \n");
#endif

#if 1
  // These tests currently fail!
  // Run AST tests (takes a while on large ASTs, so we sometime skip this for some phases of development on AST merge)
     printf ("Running AST tests (after delete) \n");
     AstTests::runAllTests(project);
     printf ("Running AST tests (after delete): DONE \n");
#endif

#if 1
  // Output performance information for ROSE plus performance information for AST Merge
     AstPerformance::generateReport();
#endif

     int errorCode = 0;

#if 0
     errorCode = backend(project);
#endif

  // End of AST Merge
     printf ("Program Terminated Normally! \n");
     return errorCode;
   }




