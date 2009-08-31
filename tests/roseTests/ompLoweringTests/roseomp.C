/*
 * A test translator to process OpenMP input
 *
 * by Liao, 8/11/2009
*/
#include "rose.h"

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  AstTests::runAllTests(project);
  return backend(project);
}

