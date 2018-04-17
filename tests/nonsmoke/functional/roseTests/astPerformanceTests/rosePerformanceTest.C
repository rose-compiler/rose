#include"rose.h"

#ifdef _MSC_VER
#include <Windows.h>
#define sleep Sleep
#endif

// #include <unistd.h>
// #include "AstPerformance.h"
// Fix suggested by Brian White (to allow "sleep()" to be defined)
// #include <unistd.h>

TimingPerformance timer ("ROSE Performance Tests: total execution time (sec) = ",true);

void foo()
   {
     TimingPerformance timer ("foo() execution time (sec) = ");
     sleep(1);
   }

void foobar()
   {
     TimingPerformance timer ("foobar() execution time (sec) = ");
     sleep(1);
   }

int
main ( int argc, char* argv[] )
   {
     TimingPerformance timer ("main() execution time (sec) = ");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
  // AstPerformance::generateReportToFile(project->get_file(0).get_sourceFileNameWithPath(),project->get_compilationPerformanceFile());
  // AstPerformance::generateReportToFile(project);
     timer.generateReportToFile(project);
   }
