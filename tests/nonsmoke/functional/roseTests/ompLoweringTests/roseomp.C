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
  const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
  generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
  return backend(project);
}

