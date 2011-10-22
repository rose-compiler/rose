/*
 * A test translator to generate IndexSet etc.
 *
 * by Liao, 8/15/2011
*/
#include "rose.h"
#include "memory_object_impl.h" // This is just one implementation. Explicit include is recommended for now.

using namespace std;
using namespace AbstractMemoryObject;
int main(int argc, char * argv[])
{
  
//  SgProject *project = frontend (argc, argv);
//  AstTests::runAllTests(project);


  // Additional test for IndexSet etc TODO
  IndexSet * s1 , *s2, *s3;
  s1 = ConstIndexSet::get_inst (10);
  s2 = ConstIndexSet::get_inst (1);
  assert (*s1 != *s2);
  s3 = UnknownIndexSet::get_inst();
  assert (*s1 == *s3);

//  return backend(project);
  return 0;
}

