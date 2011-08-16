#include "memory_object_impl.h"
#include <map>

using namespace std;

namespace AbstractMemoryObject {

 size_t Scalar_Impl::objCount() {return 1;}
 size_t LabeledAggregate_Impl::objCount() {return 1;}
 size_t Array_Impl::objCount() {return 1;}
 size_t Pointer_Impl::objCount() {return 1;}

  std::set<SgType*> ScalarAliasedObj::getType()
  {
    std::set<SgType*> rt;
    rt.insert (AliasedObj::getType());
    return rt;
  }

  std::set<SgType*> LabeledAggregateAliasedObj::getType()
  {
    std::set<SgType*> rt;
    rt.insert (AliasedObj::getType());
    return rt;
  }

  std::set<SgType*> ArrayAliasedObj::getType()
  {
    std::set<SgType*> rt;
    rt.insert (AliasedObj::getType());
    return rt;
  }

  std::set<SgType*> PointerAliasedObj::getType()
  {
    std::set<SgType*> rt;
    rt.insert (AliasedObj::getType());
    return rt;
  }



 // A map to store aliased obj set
 // This can provide quick lookup for existing aliased objset to avoid duplicated creation
  map<SgType*, ObjSet*> aliased_objset_map; 

  // builder for different objects
  ObjSet* createAliasedObjSet(SgType*t)  // One object per type, Type based alias analysis
  {
    assert (t!= NULL);
    ObjSet* rt = NULL;
    map<SgType*, ObjSet*>::const_iterator iter;
    iter = aliased_objset_map.find(t);
    if (iter == aliased_objset_map.end())
    { // None found, create a new one and update the map
      if (SageInterface::isScalarType(t))
        // We define the following SgType as scalar types: 
        // char, short, int, long , void, Wchar, Float, double, long long, string, bool, complex, imaginary 
      {
        rt = new ScalarAliasedObj(t);
      }
      else if (isSgPointerType(t))
      {
        rt = new PointerAliasedObj(t);
      }
      else if (isSgArrayType(t))
        rt = new ArrayAliasedObj (t);
      else if (isSgClassType(t))
        rt = new   LabeledAggregateAliasedObj (t);
      else
      {
        cerr<<"Warning: createAliasedObjSet(): unhandled type:"<<t->class_name()<<endl;
      }  

      // update the map 
      aliased_objset_map[t]= rt;
    }
    else // Found one, return it directly
    {
      rt = (*iter).second; 
    }  
    assert (rt != NULL);
    return rt;
  } 
  
  ObjSet* createNamedObjSet(SgSymbol* anchor_symbol, SgType* t, ObjSet* parent)
  {
     ObjSet* rt = NULL;
     return rt;

  }
  ObjSet* createExpressionObjSet(SgExpression* anchor_exp, SgType*t, ObjSet* parent)
  {
     ObjSet* rt = NULL;
     return rt;
  }

} // end namespace
