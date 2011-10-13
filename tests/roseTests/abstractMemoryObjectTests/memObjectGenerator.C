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

  // Test AliasedObj
  // ---------------------------------------------------------
  ObjSet* prev_obj = NULL;

  Rose_STL_Container <SgNode*> type_list = NodeQuery::querySubTree (project, V_SgType);
  Rose_STL_Container <SgNode*>::iterator iter;
  cout<<"Found "<<type_list.size()<<" types." <<endl;
  for (iter = type_list.begin(); iter !=type_list.end(); iter ++)
  {
    ObjSet* mem_obj = ObjSetFactory::createObjSet(*iter);
    if (mem_obj != NULL)
    {
      cout<<mem_obj->toString()<<endl;
      // test operator ==, and < for two consecutive ObjSet
      if (prev_obj != NULL)
      {
        cout<< "operator==:"<<((*prev_obj) == (*mem_obj)) <<endl;
        cout<< "operator<:"<<((*prev_obj) < (*mem_obj)) <<endl;
      }
      prev_obj = mem_obj;
    }
  }

  // Named objects from symbols 
  // ---------------------------------------------------------
  VariantVector vv (V_SgSymbol); 
  //Rose_STL_Container <SgNode*> symbol_list  = NodeQuery::querySubTree (project, V_SgSymbol);
  Rose_STL_Container <SgNode*> symbol_list  = NodeQuery::queryMemoryPool(vv);
  cout<<"----------------------------------------------------"<<endl;
  cout<<"Found "<<symbol_list.size()<<" symbols." <<endl;
  for (iter = symbol_list.begin(); iter !=symbol_list.end(); iter ++)
  {
    ObjSet* mem_obj = ObjSetFactory::createObjSet(*iter); 
    if (mem_obj != NULL)
    {
      cout<<mem_obj->toString()<<endl;
      // test operator ==, and < for two consecutive ObjSet
      if (prev_obj != NULL)
      {
        cout<< "operator==:"<<((*prev_obj) == (*mem_obj)) <<endl;
        cout<< "operator<:"<<((*prev_obj) < (*mem_obj)) <<endl;
      }
      prev_obj = mem_obj;

      // test Array::getElements (IndexVector*);  
      Array* array = dynamic_cast <Array*> (mem_obj);  
      if (array)
      {
        if (array->getNumDims()==1 )
        { // only for 1-D array, test array[0]
          cout<<"Found a 1-D array. testing array->getElements(0) ..."<<endl;
          IndexVector_Impl* myindexv = new IndexVector_Impl();
          myindexv ->index_vector.push_back(ConstIndexSet::get_inst((size_t)0));
          cout<<array->getElements(myindexv)->toString()<<endl;
        }
      }

    }
  }

  // Named objects from the SgVarRefExp part
  Rose_STL_Container <SgNode*> ref_list = NodeQuery::querySubTree (project, V_SgVarRefExp);
  cout<<"----------------------------------------------------"<<endl;
  cout<<"Found "<<ref_list.size()<<" SgVarRef." <<endl;
  for (iter = ref_list.begin(); iter != ref_list.end(); iter ++)
  {
    ObjSet* mem_obj = ObjSetFactory::createObjSet(*iter);
    if (mem_obj != NULL)
    {
      cout<<mem_obj->toString()<<endl;
      // test operator ==, and < for two consecutive ObjSet
      if (prev_obj != NULL)
      {
        cout<< "operator==:"<<((*prev_obj) == (*mem_obj)) <<endl;
        cout<< "operator<:"<<((*prev_obj) < (*mem_obj)) <<endl;
      }
      prev_obj = mem_obj;
    }
  }   

  // Other objects, expression objects, whole array objects (SgPntrArrRefExp) , or even named object for array elements
  // ---------------------------------------------------------
  Rose_STL_Container <SgNode*> exp_list = NodeQuery::querySubTree (project, V_SgExpression);
  cout<<"----------------------------------------------------"<<endl;
  cout<<"Found "<<exp_list.size()<<" SgExpression." <<endl;
  for (iter = exp_list.begin(); iter != exp_list.end(); iter ++)
  {
    ObjSet* mem_obj = ObjSetFactory::createObjSet(*iter);
    if (mem_obj != NULL)
    {
      cout<<mem_obj->toString()<<endl;
      // test operator ==, and < for two consecutive ObjSet
      if (prev_obj != NULL)
      {
        cout<< "operator==:"<<((*prev_obj) == (*mem_obj)) <<endl;
        cout<< "operator<:"<<((*prev_obj) < (*mem_obj)) <<endl;
      }
      prev_obj = mem_obj;

 
    }
 }   
  // Additional test for IndexSet etc TODO
  IndexSet * s1 , *s2, *s3;
  s1 = ConstIndexSet::get_inst (10);
  s2 = ConstIndexSet::get_inst (1);
  assert (*s1 != *s2);
  s3 = UnknownIndexSet::get_inst();
  assert (*s1 == *s3);

  return backend(project);
}

