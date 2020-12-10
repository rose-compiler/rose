/*
 *  [#125] AstPDFGeneration::generate(std::string filename, SgNode* node) crashes when iostream is included
    This tests the generation of a pdf file representing the AST.
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
     printf ("Build the AstPDFGeneration object \n");
#endif

     AstPDFGeneration pdf;

#if DEBUG_PROGRESS
     printf ("Build the PDF from the AST \n");
#endif

  // DQ (11/11/2012): Fixed filename to correspond to this test.
     pdf.generate("test125",project);

#if DEBUG_PROGRESS
     printf ("Unparse and compile code from the AST \n");
#endif

     return backend (project);
   }
