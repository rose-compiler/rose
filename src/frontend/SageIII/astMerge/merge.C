
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

#include "rose.h"

// #include "colorTraversal.h"
// #include "../../../developersScratchSpace/Dan/colorAST_tests/colorTraversal.h"
#include "../astVisualization/wholeAST_API.h"

#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH 2000
#define DISPLAY_INTERNAL_DATA 0

using namespace std;

// Global variable that functions can use to make sure that there IR nodes were not deleted!
// This is used for debugging only (tests in assertions).
set<SgNode*> finalDeleteSet;


// void mergeAST ( SgProject* project )
void
mergeAST ( SgProject* project, bool skipFrontendSpecificIRnodes )
   {
  // DQ (5/31/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("AST merge:");

  // DQ (5/27/2007): Implement this as a local variable!
  // set<SgNode*> finalDeleteSet;

  // Note that skipFrontendSpecificIRnodes alows the generated graphs to skip the 
  // representation or IR nodes that are marked to be frontend specific.

     printf ("\n\n");
     printf ("**************************************************************** \n");
     printf ("**********************  START AST MERGE ************************ \n");
     printf ("**************************************************************** \n");

  // DQ (5/31/2007): Force this to be true for testing AST merge
     ROSE_ASSERT(skipFrontendSpecificIRnodes == true);

  // Generate the filename to be used as a base for generated dot files (graphs)
     string filename = SageInterface::generateProjectName(project);

     int numberOfASTnodesBeforeMerge = numberOfNodes();
     printf ("numberOfASTnodesBeforeMerge = %d MAX_NUMBER_OF_IR_NODES_TO_GRAPH = %d \n",numberOfASTnodesBeforeMerge,MAX_NUMBER_OF_IR_NODES_TO_GRAPH);
     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH)
        {
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMerge");
          set<SgNode*> emptySet;
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMergeWholeAST_unfiltered",emptySet);
          generateWholeGraphOfAST(filename+"_beforeMergeWholeAST_unfiltered",emptySet);
          set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMergeWholeAST",skippedNodeSet);
          generateWholeGraphOfAST(filename+"_beforeMergeWholeAST",skippedNodeSet);
       // SimpleColorFilesTraversal::generateGraph(project,filename+"_beforeMerge",skippedNodeSet);
          generateGraphOfAST(project,filename+"_beforeMerge",skippedNodeSet);
        }

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

     printf ("mangledNameMap.size() = %zu intermediateDeleteSet = %zu \n",mangledNameMap.size(),intermediateDeleteSet.size());

#if DISPLAY_INTERNAL_DATA 
     printf ("\n\n**************************************** \n");
     printf ("mangledNameMap computed by getMangledNameMap \n");
     printf ("**************************************** \n");
     MangledNameMapTraversal::displayMagledNameMap(mangledNameMap);
#endif

#if DISPLAY_INTERNAL_DATA > 1

     printf ("\n\n**************************************** \n");
     printf ("Delete set computed by getMangledNameMap \n");
     printf ("**************************************** \n");
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
          printf ("Generate the graph after the copy intermediateDeleteSet = %zu \n",intermediateDeleteSet.size());
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMangledNameMapWholeAST",intermediateDeleteSet);
          generateWholeGraphOfAST(filename+"_afterMangledNameMapWholeAST",intermediateDeleteSet);
       // SimpleColorFilesTraversal::generateGraph(project,filename+"_afterMangledNameMap");
          generateGraphOfAST(project,filename+"_afterMangledNameMap");
        }

#if 0
     printf ("Exiting as part of test ... \n");
     exit(1);
#endif

     int numberOfASTnodesAfterCopy = numberOfNodes();
     printf ("After AST copy: numberOfASTnodesAfterCopy = %d (%d increase) intermediateDeleteSet = %ld \n",
          numberOfASTnodesAfterCopy,numberOfASTnodesAfterCopy-numberOfASTnodesBeforeCopy,(long int)intermediateDeleteSet.size());

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
#if DISPLAY_INTERNAL_DATA
     printf ("\n\n After replacementMapTraversal(): replacementMap: \n");
     ReplacementMapTraversal::displayReplacementMap(replacementMap);

     printf ("\n\n After replacementMapTraversal(): intermediateDeleteSet: \n");
     displaySet(intermediateDeleteSet,"After replacementMapTraversal");
#endif

  // TestParentPointersOfSymbols::test();

     printf ("After replacementMapTraversal: replacementMap = %ld intermediateDeleteSet = %ld \n",(long)replacementMap.size(),(long)intermediateDeleteSet.size());

#if 0
     printf ("Exiting as part of test after computing the replacementMap ... \n");
     exit(1);
#endif

     printf ("Calling ReplacementMapTraversal::buildListOfODRviolations() \n");
     set<SgNode*> problemSubtreeSet = ReplacementMapTraversal::buildListOfODRviolations(ODR_Violations);
     printf ("Calling ReplacementMapTraversal::buildListOfODRviolations(): DONE \n");

     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH)
        {
          printf ("Generate the graph after computing the replacement map (ODR_Violations.size() = %ld) \n",(long)ODR_Violations.size());
       // SimpleColorFilesTraversal::generateGraph(project,filename+"_ODR_violations_afterReplacementMap",problemSubtreeSet);
          generateGraphOfAST(project,filename+"_ODR_violations_afterReplacementMap",problemSubtreeSet);
          set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
       // SimpleColorFilesTraversal::generateGraph(project,filename+"afterReplacementMap",skippedNodeSet);
          generateGraphOfAST(project,filename+"afterReplacementMap",skippedNodeSet);
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
#if DISPLAY_INTERNAL_DATA > 1
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
          printf ("Generate the graph after the merge intermediateDeleteSet = %ld \n",(long)intermediateDeleteSet.size());
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST",intermediateDeleteSet);
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST",getSetOfFrontendSpecificNodes(requiredNodesSet));
       // set<SgNode*> emptySet;
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST_unfiltered",emptySet);
          generateWholeGraphOfAST(filename+"_afterMergeWholeAST_unfiltered");
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterMergeWholeAST",getSetOfFrontendSpecificNodes());
          set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
          generateWholeGraphOfAST(filename+"_afterMergeWholeAST",skippedNodeSet);
       // SimpleColorFilesTraversal::generateGraph(project,filename+"_afterMerge");
          generateGraphOfAST(project,filename+"_afterMerge");
        }

     int numberOfASTnodesBeforeDelete = numberOfNodes();
     printf ("Before AST delete: numberOfASTnodesBeforeDelete = %d intermediateDeleteSet = %ld finalDeleteSet = %ld \n",
          numberOfASTnodesBeforeDelete,(long)intermediateDeleteSet.size(),(long)finalDeleteSet.size());

  // ****************************************************************************
  // *****************  Compute Final Set of IR Nodes To Delete  ****************
  // ****************************************************************************
// Using the intermediateDeleteSet and the requiredNodesSet
     set<SgNode*> requiredNodesSet = buildRequiredNodeList(project);
#if DISPLAY_INTERNAL_DATA > 1
     displaySet (requiredNodesSet,"After fixupTraversal(): requiredNodesSet");
#endif

  // displayDeleteSet (intermediateDeleteSet);

     printf ("requiredNodesSet.size() = %ld \n",(long)requiredNodesSet.size());

#if 0
     printf ("Exiting after computing required IR nodes ... \n");
     exit(1);
#endif

     printf ("Calling computeSetDifference() \n");
  // Computer the set difference between the nodes to delete and the nodes that are required!
  // set<SgNode*> finalDeleteSet = computeSetDifference(intermediateDeleteSet,requiredNodesSet);
     finalDeleteSet = computeSetDifference(intermediateDeleteSet,requiredNodesSet);
     printf ("Calling compute set difference(): DONE \n");

     printf ("Output the set difference size = %ld \n",(long)finalDeleteSet.size());

#if DISPLAY_INTERNAL_DATA
     displaySet(finalDeleteSet,"finalDeleteSet");
#endif

  // DQ (2/15/2007): Error checking on the finalDeleteSet
  // deleteSetErrorCheck( project, requiredNodesSet );
     deleteSetErrorCheck( project, finalDeleteSet );

     printf ("Calling computeSetIntersection() \n");
     set<SgNode*> intersectionSet = computeSetIntersection(intermediateDeleteSet,requiredNodesSet);
     printf ("Calling computeSetIntersection(): DONE \n");

     printf ("intersectionSet.size() = %ld \n",(long)intersectionSet.size());
#if DISPLAY_INTERNAL_DATA > 1
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
     printf ("skippedNodeSet size = %ld \n",(long)skippedNodeSet.size());
  // displaySet(skippedNodeSet,"result from getSetOfFrontendSpecificNodes() function");

  // set<SgNode*> skippedNodeSet          = sharedNodeSet;
  // skippedNodeSet.insert(frontendSpecificNodeSet.begin(),frontendSpecificNodeSet.end());
  // set<SgNode*> skippedNodeSet          = frontendSpecificNodeSet;


  // printf ("sharedNodeSet size = %ld frontendSpecificNodeSet size = %ld skippedNodeSet size = %ld \n",sharedNodeSet.size(),frontendSpecificNodeSet.size(),skippedNodeSet.size());

#if 0
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
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_afterDeleteWholeAST",skippedNodeSet);
          generateWholeGraphOfAST(filename+"_afterDeleteWholeAST",skippedNodeSet);
       // SimpleColorFilesTraversal::generateGraph(project,filename+"_ODRviolations",problemSubtreeSet);
        }

#if 0
  // These tests currently fail!
  // Run AST tests (takes a while on large ASTs, so we sometime skip this for some phases of development on AST merge)
     printf ("Running AST tests (after delete, and before graph generation) \n");
     AstTests::runAllTests(project);
     printf ("Running AST tests (after delete, and before graph generation): DONE \n");
#else
     printf ("Skipping AST tests (after delete, and before graph generation): DONE \n");
#endif

  // ******************************************************************************
  // ****************** Compute Efficency of AST Merge Mechanism ******************
  // ******************************************************************************
     printf ("numberOfASTnodesBeforeMerge = %d numberOfASTnodesBeforeDelete = %d \n",numberOfASTnodesBeforeMerge,numberOfASTnodesBeforeDelete);
     double percentageDecrease = 100.0 - ( ((double) numberOfASTnodesAfterDelete) / ((double) numberOfASTnodesBeforeDelete) ) * 100.0;
     double numberOfFiles = project->numberOfFiles();
     double mergeEfficency = 0.0;
     if (numberOfFiles == 1)
        {
       // For a single file we expect no decrease to ge 100% efficiency (but we do get something for better sharing of types)
          mergeEfficency = 1.0 + percentageDecrease;
        }
       else
        {
          double fileNumberMultiplier = numberOfFiles / (numberOfFiles - 1);
          mergeEfficency = percentageDecrease * fileNumberMultiplier;
        }

     printf ("\n\n");
     printf ("******************************************************************************************************************************************************************** \n");
     printf ("After AST delete: numberOfASTnodesBeforeMerge = %d numberOfASTnodesAfterDelete = %d (%d node decrease: %2.4lf percent decrease, mergeEfficency = %2.4lf) \n",
          numberOfASTnodesBeforeMerge,numberOfASTnodesAfterDelete,numberOfASTnodesBeforeDelete-numberOfASTnodesAfterDelete,percentageDecrease,mergeEfficency);
     printf ("******************************************************************************************************************************************************************** \n\n\n");

#if 0
     reportUnsharedDeclarationsTraversal();
#else
     printf ("Note: Skipping reportUnsharedDeclarationsTraversal(): generates a list of un-shared constructs \n");
#endif

#if 0
  // These tests currently fail!
  // Run AST tests (takes a while on large ASTs, so we sometime skip this for some phases of development on AST merge)
     printf ("Running AST tests (after delete) \n");
     AstTests::runAllTests(project);
     printf ("Running AST tests (after delete): DONE \n");
#else
     printf ("Skipping AST tests (after delete): DONE \n");
#endif

   }


int buildAstMergeCommandFile ( SgProject* project )
   {
     vector<string> argv = project->get_originalCommandLineArgumentList();
     int errorCode = 0;

  // If using astMerge mechanism we have to save the command line and working directories to a separate file.
  // ROSE_ASSERT(project->get_astMerge() == true);

  // If we are saving the comment lines then we are not yet merging the AST (so get_astMerge() == false)
     ROSE_ASSERT(project->get_astMerge() == false);

     string astMergeCommandFilename = project->get_astMergeCommandFile();
     ROSE_ASSERT(astMergeCommandFilename != "");

     ofstream astMergeSupportFile ( astMergeCommandFilename.c_str() , ios::out | ios::app );
     if ( astMergeSupportFile.good() == false )
        {
          printf ("astMergeCommandFilename = %s failed to open \n",astMergeCommandFilename.c_str());
        }

     bool skipInitialEntry = false;
     bool skipSourceFiles  = false;


  // string commandline = CommandlineProcessing::generateStringFromArgcArgv(
  //                           SgProject::project_argc,SgProject::project_argv,skipInitialEntry,skipSourceFiles);
     string commandline = CommandlineProcessing::generateStringFromArgList(argv,skipInitialEntry,skipSourceFiles);
     string separator = "::";
     string workingDirectory = ROSE::getWorkingDirectory();

     printf ("commandline      = %s \n",commandline.c_str());
     printf ("separator        = %s \n",separator.c_str());
     printf ("workingDirectory = %s \n",workingDirectory.c_str());

     string combinedDirectoryAndCommandline = workingDirectory + separator + commandline;

     printf ("combinedDirectoryAndCommandline = %s \n",combinedDirectoryAndCommandline.c_str());

     astMergeSupportFile << combinedDirectoryAndCommandline << endl;

     printf ("Closing file %s in setupAstMerge \n",astMergeCommandFilename.c_str());
     astMergeSupportFile.close();

     printf ("Leaving setupAstMerge \n");

     return errorCode;
   }


int AstMergeSupport ( SgProject* project )
   {
     printf ("Inside of AstMergeSupport \n");

     int errorCode = 0;
     string astMergeCommandFilename = project->get_astMergeCommandFile();

     if (astMergeCommandFilename.empty() == true)
        {
       // DQ (5/26/2007): New case added, specification of astMerge without a command-line file name does not have to be an error.
       // This case allows either a single file to be better merged (with itself, which usally results in a 20% memory savings,
       // or with other files specificed explicitly on the command-line).  This should make it easier to test the AST merge in ROSE.

          printf ("Note: -rose:astMerge specified without -rose:astMergeCommandFile filename \n");

          ROSE_ASSERT (project->get_sourceFileNameList().size() > 0);

       // DQ (5/26/2007): output message to user about what AST merge will be doing!
          if (project->get_sourceFileNameList().size() == 1)
             {
            // specification of a single file ...
               printf ("This case will cause a single file to be merged with itself, saving come from better sharing of the types. \n");
             }
            else
             {
            // specification of multiple files ...
               printf ("This case will merge multiple files on the commandline to be merged sourceFileNameList().size() = %ld \n",(long)(project->get_sourceFileNameList().size()));
             }

          errorCode = project->parse();

          bool skipFrontendSpecificIRnodes = true;
          printf ("Calling mergeAST \n");

          mergeAST(project,skipFrontendSpecificIRnodes);
        }
       else
        {
       // DQ (5/26/2007): This used to be the only case processed.
          ROSE_ASSERT(astMergeCommandFilename.empty() == false);
          ROSE_ASSERT(astMergeCommandFilename != "");

          printf ("Inside of AstMergeSupport opening file %s \n",astMergeCommandFilename.c_str());

          ifstream astMergeSupportFile ( astMergeCommandFilename.c_str() , ios::in );
          if ( astMergeSupportFile.good() == false )
             {
               printf ("astMergeCommandFilename = %s failed to open \n",astMergeCommandFilename.c_str());
               ROSE_ASSERT(false);
             }

          while (!astMergeSupportFile.eof())
             {
#if 0
            // DQ (6/1/2007): This actually tries to read lines longer than 5000 char and of course truncates in these cases.
            // It is not a buffer overflow, but it does fail to read until the end of the line and so causes errors down stream.
               char buffer[5000];
               astMergeSupportFile.getline(buffer,5000);
               string fileString = buffer;
#else
            // DQ (6/1/2007): This case uses the std::getline() function defined in <string> header file.
            // This is better and more useful since it avoids any specification of limit in the string size.
               string fileString;
               getline(astMergeSupportFile,fileString);

            // Error checking on read from file (eofbit may be true or false, we don't have to reach the eof in each read from the file)
            // ROSE_ASSERT(astMergeSupportFile.eofbit == true);
            // DQ (6/1/2007): It appears that this can be true sometimes (perhaps in the eofbit == true case), so skip testing thes for now.
            // ROSE_ASSERT(astMergeSupportFile.failbit == false);
            // ROSE_ASSERT(astMergeSupportFile.badbit == false);               
#endif

            // The last line of the file does not yet trigger astMergeSupportFile.eof()
               if (fileString.size() > 0)
                  {
                    printf ("fileString = %s \n",fileString.c_str());

                    string separator = "::";
                    std::string::size_type separatorPosition = fileString.find("::");
                    ROSE_ASSERT ( separatorPosition != string::npos );

                    string workingDirectory = fileString.substr(0,separatorPosition);
                    separatorPosition += separator.size();
                    string commandline = fileString.substr(separatorPosition,fileString.size());

                    printf ("commandline      = %s \n",commandline.c_str());
                    printf ("separator        = %s \n",separator.c_str());
                    printf ("workingDirectory = %s \n",workingDirectory.c_str());

                    Rose_STL_Container<string> argList = CommandlineProcessing::generateArgListFromString(commandline);

                    char** argv = NULL;
                    int argc = 0;
                    CommandlineProcessing::generateArgcArgvFromList(argList,argc,argv);

                    int nextErrorCode = 0;
#if 1
                 // int fileIndex = 0;
                    SgFile* newFile = determineFileType( vector<string>(argv, argv+argc), nextErrorCode,  project );
                    ROSE_ASSERT (newFile != NULL);

                    newFile->set_parent(project);
                    project->set_file ( *newFile );
#endif
                    errorCode = errorCode >= nextErrorCode ? errorCode : nextErrorCode;
                  }
             }

#if 0
       // Build the AST Merge object (this is not the final interface)
          AstMerge mergeSupport;
          mergeSupport.addAST(project);
#else
       // DQ (5/26/2007): New interface
          bool skipFrontendSpecificIRnodes = true;
          mergeAST(project,skipFrontendSpecificIRnodes);
#endif
        }

     printf ("Leaving AstMergeSupport \n");

     return errorCode;
   }



