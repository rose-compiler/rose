#include "rose.h"
#include <string>

using namespace std;

int
main(int argc, char **argv)
{
  SgProject *project = frontend(argc, argv);
  if (project != NULL) {
    string foo = project->unparseToString();
  }
  return 0;
}
