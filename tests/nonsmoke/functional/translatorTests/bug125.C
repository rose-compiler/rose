/*
 *  [#125] AstPDFGeneration::generate(std::string filename, SgNode* node) crashes when iostream is included
    This tests the generation of a pdf file representing the AST.
 */

#include "rose.h"
#include <iostream>
using namespace std;

int
main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

     AstTests::runAllTests(project);

     AstPDFGeneration pdf;

  // DQ (11/11/2012): Fixed filename to correspond to this test.
     pdf.generate("test125",project);

     return backend (project);
   }
