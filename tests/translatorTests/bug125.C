/*
 *  [#125] AstPDFGeneration::generate(std::string filename, SgNode* node) crashes when iostream is included
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
 pdf.generate("test111",project);

  return backend (project);
}
