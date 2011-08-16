/*
 * A test translator to generate memory objects
 *
 * by Liao, 8/15/2011
*/
#include "rose.h"

using namespace std;
using namespace AbstractMemoryObject;
int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  AstTests::runAllTests(project);

  SgTypeTable * t_table = SgNode::get_globalTypeTable();
  SgSymbolTable * s_table = t_table->get_type_table();
  std::set<SgNode*> symbols = s_table->get_symbols();

  std::set<SgNode*>::iterator iter;
  for (iter = symbols.begin(); iter!= symbols.end(); iter++)
  {
    SgNode* n = *iter;  
    cout<<n->class_name()<<endl; // all SgFunctionTypeSymbol
  }
  return backend(project);
}

