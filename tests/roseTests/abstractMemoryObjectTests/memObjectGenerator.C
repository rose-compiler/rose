/*
 * A test translator to generate memory objects
 *
 * by Liao, 8/15/2011
*/
#include "rose.h"
#include "memory_object_impl.h" // This is just one implementation. Explicit include is recommended for now.

using namespace std;
using namespace AbstractMemoryObject;
int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  AstTests::runAllTests(project);

#if 0
  SgTypeTable * t_table = SgNode::get_globalTypeTable();
  SgSymbolTable * s_table = t_table->get_type_table();
  std::set<SgNode*> symbols = s_table->get_symbols();

  std::set<SgNode*>::iterator iter;
  for (iter = symbols.begin(); iter!= symbols.end(); iter++)
  {
    SgNode* n = *iter;  
    cout<<n->class_name()<<endl; // all SgFunctionTypeSymbol
  }
#endif
  Rose_STL_Container <SgNode*> type_list = NodeQuery::querySubTree (project, V_SgType);
   Rose_STL_Container <SgNode*>::iterator iter;
   cout<<"Found "<<type_list.size()<<" types." <<endl;
   for (iter = type_list.begin(); iter !=type_list.end(); iter ++)
   {
     SgType* t = isSgType(*iter);
//     cout<<t->class_name()<<endl;
     ObjSet* mem_obj = createAliasedObjSet (t);
     if (mem_obj != NULL)
       cout<<mem_obj->toString()<<endl;
   }
  return backend(project);
}

