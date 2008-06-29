// This example demonstrates the seeding of a specific type
// of bug (buffer overflow) into any existing application to 
// test bug finding tools.

#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeedingSupport.h"

int
main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

  // Running internal tests (optional)
     AstTests::runAllTests (project);

  // Build a BufferOverFlowSecurityFlaw object
     BufferOverFlowSecurityFlaw bufferOverFlowSecurityFlaw;

  // Call the member function to annotate the AST where BufferOverFlowSecurityFlaw vulnerabilities exist.
     bufferOverFlowSecurityFlaw.detectVunerabilities(project);

  // Call the member function to seed security flaws into the AST (at locations where vulnerabilities were previously detected).
     bufferOverFlowSecurityFlaw.seedSecurityFlaws(project);

  // Running internal tests (optional)
     AstTests::runAllTests (project);

  // Output the new code seeded with a specific form of bug.
     return backend (project);
   }
