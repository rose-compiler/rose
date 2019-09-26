// Example ROSE Translator reads input program and tests AST and WholeAST graph generation.
#include "rose.h"

// Options to generate graphs with and without filering of
// IR nodes can be use to tailor the generated graph output.
// Example graph options are:
//      -rose:dotgraph:expressionFilter 1
//      -rose:dotgraph:fileInfoFilter 0 

int main( int argc, char * argv[] )
   {
     TimingPerformance timer ("main() execution time (sec) = ");

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
  // AstTests::runAllTests(project);

#if 0
  // DQ (11/20/2013): Test using support for multiple files for Java testing.
  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT ( *project );
     generateDOTforMultipleFile(*project);
#else
     AstDOTGeneration astdotgen;
  // SgProject & nonconstProject = (SgProject &) project;
     std::string filenamePostfix;

  // DQ (6/1/2019): Uncommented to debug multiple file support.
  // astdotgen.generateInputFiles(project,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);

     SgFile* file = project->get_files()[0];
     ROSE_ASSERT(file != NULL);

  // DQ (6/1/2019): Commented out to debug multiple file support.
     astdotgen.generateWithinFile(file,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);
#endif

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");

#if 0
  // DQ (8/20/2013): Added initial support for IR node specific DOT graphs.
  // std::vector<VariantT> nodeKindList = {V_SgTemplateFunctionDeclaration,V_SgTemplateMemberFunctionDeclaration};
  // VariantT initializationArray[3] = {V_SgInitializedName,V_SgTemplateFunctionDeclaration,V_SgTemplateMemberFunctionDeclaration};
     VariantT initializationArray[3] = {V_SgDeclarationStatement,V_SgTemplateFunctionDeclaration,V_SgTemplateMemberFunctionDeclaration};
     std::vector<VariantT> nodeKindList(initializationArray, initializationArray+1);
     intermediateRepresentationNodeGraph(project,nodeKindList);
#endif

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
  // return backend(project);
     int exit_status = backend(project);

  // DQ (6/30/2013): Compute the elapsed time to this point.
     timer.endTimer();

  // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
  // AstPerformance::generateReportToFile(project->get_file(0).get_sourceFileNameWithPath(),project->get_compilationPerformanceFile());
  // AstPerformance::generateReportToFile(project);
     timer.generateReportToFile(project);

  // DQ (12/12/2009): Allow output only when run in verbose mode to limit spew in testing.
     if (SgProject::get_verbose() > 0)
        {
          int memoryUsageSize = memoryUsage();
          printf ("Alternative output from memoryUsage() = %d \n",memoryUsageSize);

          printf ("Calling AstNodeStatistics::traversalStatistics(project) \n");
          std::cout << AstNodeStatistics::traversalStatistics(project);

          printf ("Calling AstNodeStatistics::IRnodeUsageStatistics \n");
          std::cout << AstNodeStatistics::IRnodeUsageStatistics();

       // DQ (2/17/2013): Added performance report (reports performance timings and memory usage for many different parts of ROSE).
          timer.generateReportFromObject();
        }

     return exit_status;
   }

