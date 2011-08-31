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

  // Test AliasedObj
  AliasedObj* prev_obj = NULL;

  Rose_STL_Container <SgNode*> type_list = NodeQuery::querySubTree (project, V_SgType);
  Rose_STL_Container <SgNode*>::iterator iter;
  cout<<"Found "<<type_list.size()<<" types." <<endl;
  for (iter = type_list.begin(); iter !=type_list.end(); iter ++)
  {
    SgType* t = isSgType(*iter);
    //     cout<<t->class_name()<<endl;
    ObjSet* mem_obj = createAliasedObjSet (t);
    if (mem_obj != NULL)
    {
      cout<<mem_obj->toString()<<endl;

      // test operator == for two consecutive aliased ObjSet
      if (prev_obj != NULL)
      {
        cout<< "operator==:"<<((*prev_obj) == (*mem_obj)) ;
        cout<< "  operator<:"<<((*prev_obj) < (*(dynamic_cast <AliasedObj*> (mem_obj)))) <<endl;
      }

      prev_obj = dynamic_cast <AliasedObj*> (mem_obj);
    }
  }
  
  //Test NamedObj
  //
  //there are at least three levels resolution for modeling memory for labeled aggregates (structures, classes, etc)
  //
  //Think the following example:
  //-------------------
  //struct A { int i; int j;} ;
  //struct A a1;
  //struct A a2;
  //
  //int x = a1.i + a2.i + a2.j + a2.j;
  //-------------------
  //
  //1. most coarse: treat any references to struct A's instance as the same to struct A.
  //    This means to treat a1.i, a2.i, a2.j, a2.j as the same access to struct A.
  //    There is only one memory object for all a1.i, a2.i, a2.j and a2.j.
  //
  //2. finer: distinguish between different fields of A. But the same field to all instances will be treated as the same.
  //
  //  Now a1.i and a2.i will be modeled as A.i
  //      a2.j and a2.j will be modeled as A.j
  //  There are two memory objects for all a1.i, a2.i, a2.j and a2.j.
  //
  //3. Finest : distinguish between not only the fields, but also the instances.
  //
  //    a1.i, a2.i, a2.j, a2.j will be represented as four different objects.
  
  // We decide the resolution of this implementation is to distinguish between both fields and instances of aggregated objects.
  // This means type declarations (e.g. struct A{}) without declared instances (struct A a1; )will not have the corresponding memory objects.
  //
  // Naively creating NamedObj from symbol won't work since a single symbol can be shared by different NamedObj,
  // e.g.,  a1.i, and a2.i are different. But the field i will share the same symbol
  // So we have to traverse the SgVarRef to generate NamedObj
  // three level case: a1.mag.size  the AST will look like
  //    (a1 <-- SgDotExp --> mag ) <-- SgDotExp --> size
  //
  // To create all NamedObj, we search both symbols and SgVarRef and bypass the overlapped objects.
  //
  // For all symbols, there are two categories
  // 1. symbols corresponding to real top level instances of types. Create NamedObj as we see each of them, NULL as parent
  //    They are the symbols with declarations not under SgClassDefinition
  // 2. symbols within type declarations: does not correspond NamedObj by itself. We should ignore them until we see the instance  
  //    They are the symbols with declarations under SgClassDefinition
  //   
  // For all SgVarRef, find the corresponding symbol 
  // 1. if is a instance symbol. skip since we already created it
  // 2. if it is a symbol within a declaration, two cases
  //    a. the symbol has a pointer type, we don't track pointer aliasing, so we create AliasedObj for it
  //    b. other types: a child of an instance, check if is the rhs of SgDotExp/SgArrowExp, if not assert
  //        use lhs of SgDotExp/SgArrowExp as parent
  //            lhs could be SgVarRefExp: find the corresponding NamedObj as parent (top level object, labeled aggregate)
  //            lhs could be another SgDotExp: find its rhs's NamedObj as parent
  //  
  //  The final consideration, avoid duplicated NamedObj, map[parent][symbol] -> NamedObj
  //
  // TODO: build the connection between SgVarRefExp and the created NamedObj and support fast lookup!
  VariantVector vv (V_SgSymbol); 
  //Rose_STL_Container <SgNode*> symbol_list  = NodeQuery::querySubTree (project, V_SgSymbol);
  Rose_STL_Container <SgNode*> symbol_list  = NodeQuery::queryMemoryPool(vv);
  cout<<"----------------------------------------------------"<<endl;
  cout<<"Found "<<symbol_list.size()<<" symbols." <<endl;
  for (iter = symbol_list.begin(); iter !=symbol_list.end(); iter ++)
  {
    SgSymbol* t = isSgSymbol(*iter);
    //     cout<<t->class_name()<<endl;
    if (!isMemberVariableDeclarationSymbol (t))
    {
      ObjSet* mem_obj = createNamedObjSet (t, t->get_type(), NULL); //TODO parent case??
      if (mem_obj != NULL)
      {
        cout<<mem_obj->toString()<<endl;
#if 0
        // test operator == for two consecutive aliased ObjSet
        if (prev_obj != NULL)
        {
          cout<< "operator==:"<<((*prev_obj) == (*mem_obj)) ;
          cout<< "  operator<:"<<((*prev_obj) < (*(dynamic_cast <AliasedObj*> (mem_obj)))) <<endl;
        }
        prev_obj = dynamic_cast <AliasedObj*> (mem_obj);
#endif

      }
    }
  }
  // now the SgVarRefExp part
  // assumption here is that NodeQuery::querySubTree() uses preorder traversal so a mem obj for a parent part (e.g., a.b)
  // is always created before it is used to create a child part's mem obj (e.g., a.b.c)
  Rose_STL_Container <SgNode*> ref_list = NodeQuery::querySubTree (project, V_SgVarRefExp);
  cout<<"----------------------------------------------------"<<endl;
  cout<<"Found "<<ref_list.size()<<" SgVarRef." <<endl;
  for (iter = ref_list.begin(); iter != ref_list.end(); iter ++)
  {
    SgVarRefExp* r = isSgVarRefExp (*iter);
    assert (r!=NULL);
    ObjSet* mem_obj = createObjSet (r);
    if (mem_obj != NULL)
      cout<<mem_obj->toString()<<endl;
  }   

  return backend(project);
}

