/*
 *  [#125] AstPDFGeneration::generate(std::string filename, SgNode* node) crashes when iostream is included
    This tests the generation of a pdf file representing the AST.

    9/2022 [MS] : PDF generation has been replaced with JSON generation.  Retaining this test for the
    time being but it may no longer be necessary if the underlying bug was related to PDFs only.
 */

#include "rose.h"
#include <iostream>
using namespace std;

#define DEBUG_PROGRESS 1

int
main (int argc, char *argv[])
   {
#if DEBUG_PROGRESS
     printf ("Build the AST \n");
#endif

     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

#if DEBUG_PROGRESS
     printf ("Calling AstTests::runAllTests(project) \n");
#endif

     AstTests::runAllTests(project);

#if DEBUG_PROGRESS
     printf ("Build the AstJSONGeneration object \n");
#endif

     AstJSONGeneration json;

#if DEBUG_PROGRESS
     printf ("Build the JSON from the AST \n");
#endif

  // DQ (11/11/2012): Fixed filename to correspond to this test.
     json.generate("test125",project);

#if DEBUG_PROGRESS
     printf ("Unparse and compile code from the AST \n");
#endif

     return backend (project);
   }
