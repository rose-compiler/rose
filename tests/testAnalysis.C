// Example ROSE Translator
// used for testing ROSE infrastructure

#include "rose.h"

int
main( int argc, char * argv[] )
   {
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST translation (main): time (sec) = ",true);

     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);

  // Output statistics about how ROSE was used...
     if (project->get_verbose() > 1)
        {
       // cout << stat.toString(project);
          std::cout << AstNodeStatistics::traversalStatistics(project);
          std::cout << AstNodeStatistics::IRnodeUsageStatistics();
        }

  // Just set the project, the report will be generated upon calling the destructor for "timer"
  // Use option "-rose:verbose 2" to see the report.
     timer.set_project(project);

  // Skip calling the typical backend for ROSE (not required for just testing analysis)
  // This backend calls the backend compiler using the original input source file list.
     return backendUsingOriginalInputFile(project);
   }
