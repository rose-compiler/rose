#include <unistd.h>
#include "AstPerformance.h"
#include <cstdio>
// Fix suggested by Brian White (to allow "sleep()" to be defined)
// #include <unistd.h>

TimingPerformance timer ("ROSE Performance Tests: total execution time (sec) = ",true);

void foo()
   {
     TimingPerformance timer ("foo() execution time (sec) = ");

     printf ("Before memory allocation ... \n");
  // Allocate some memory to test the memory useage
     for (int i=0; i < 1000; i++)
        {
          /* int* pointer = */ new int[250000];
        }

     printf ("After memory allocation ... \n");

     sleep(1);
   }

void foobar()
   {
     TimingPerformance timer ("foobar() execution time (sec) = ");
     sleep(1);
   }

int main()
   {
     TimingPerformance timer ("main() execution time (sec) = ");
     sleep(1);

     printf ("Before memory allocation ... \n");
  // Allocate some memory to test the memory useage
     for (int i=0; i < 1000; i++)
        {
          /* int* pointer = */ new int[250000];
        }

     printf ("After memory allocation ... \n");

     foo();
     TimingPerformance nested_timer ("another timer in main: execution time (sec) = ");
     foobar();

  // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
  // AstPerformance::generateReport();
   }
