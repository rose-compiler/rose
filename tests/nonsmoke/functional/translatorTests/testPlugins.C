/*
 * Test the support for plugins
*/
#include "rose.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  return backend(project);
}
