#include "memory_object_impl.h"
#include <map>
#include <typeinfo>

using namespace std;

namespace AbstractMemoryObject {

 // They are all single object
 size_t Scalar_Impl::objCount() {return 1;}
 size_t LabeledAggregate_Impl::objCount() {return 1;}
 size_t Array_Impl::objCount() {return 1;}
 size_t Pointer_Impl::objCount() {return 1;}

 bool Scalar_Impl::maySet() { return false; }
 bool LabeledAggregate_Impl::maySet() { return false; }
 bool Array_Impl::maySet() { return false; }
 bool Pointer_Impl::maySet() { return false; }

 bool Scalar_Impl::mustSet() { return true; }
 bool LabeledAggregate_Impl::mustSet() { return true; }
 bool Array_Impl::mustSet() { return true; }
 bool Pointer_Impl::mustSet() { return true; }


 std::string AliasedObj::toString()  
 {
   string rt;
   if (type != NULL )
     rt += type->unparseToString() + " @ " + StringUtility::numberToString(type); 
   return rt;
 } 

  bool AliasedObj::operator == ( AliasedObj & o2) 
 {
   AliasedObj& o1 = *this;
   SgType* own_type = o1.getType();
   SgType* other_type = o2.getType();
  
   // Simplest alias analysis: same type ==> aliased
   // TODO : consider subtype, if type1 is a subtype of type2, they are aliased to each other also
   if (own_type == other_type)
     return true;
   else
     return false;
 }

 // TODO AliasedObj and NamedObj,  
 // TODO NamedObj and AliasedObj 
 bool AliasedObj::operator < ( AliasedObj& o2)
 {
   return (this->getType() < o2.getType());
 }

 bool AliasedObj::operator == (ObjSet& o2) 
 {
   // three cases
   // 1. o2 is  ExpressionObj: always return false
   // 2. o2 is Named Obj: return operator == (AliasedObj&o1, NamedObj & o2)
   // 3. o2 is AliasedObj:
   bool is_exp_obj = true;
   bool is_named_obj = true;
   bool is_aliased_obj = true;

   // There might be better way to code this
   try
   {
     AliasedObj & aliased_o2 = dynamic_cast <AliasedObj&> (o2); 
   } 
   catch (bad_cast & bc)
   {
     is_aliased_obj = false;
   }

/*
   try
   {
     ExpressionObj & exp_o2 = dynamic_cast <ExpressionObj&> (o2); 
   } 
   catch (bad_cast & bc)
   {
     is_exp_obj = false;
   }
*/
    try
   {
     NamedObj& named_o2 = dynamic_cast <NamedObj&> (o2); 
   } 
   catch (bad_cast & bc)
   {
     is_named_obj = false;
   }

   AliasedObj& o1 = *this;
   if (is_aliased_obj)
   {
     AliasedObj & aliased_o2 = dynamic_cast <AliasedObj&> (o2);
     return o1 == aliased_o2;
   }  

   // TODO other two cases
   return false;
 }

 bool ScalarAliasedObj::operator == (ObjSet& o2)
 {
   AliasedObj& o1 = dynamic_cast<AliasedObj&> (*this);
 //  return (o1.operator==( o2));
   return (o1==o2);
 }

 bool LabeledAggregateAliasedObj::operator == (ObjSet& o2)
 {
   AliasedObj& o1 = dynamic_cast<AliasedObj&> (*this);
 //  return (o1.operator==( o2));
   return (o1==o2);
 }

 bool ArrayAliasedObj::operator == (ObjSet& o2)
 {
   AliasedObj& o1 = dynamic_cast<AliasedObj&> (*this);
 //  return (o1.operator==( o2));
   return (o1==o2);
 }

 bool PointerAliasedObj::operator == (ObjSet& o2)
 {
   AliasedObj& o1 = dynamic_cast<AliasedObj&> (*this);
 //  return (o1.operator==( o2));
   return (o1==o2);
 }



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

  // toString()
  string ScalarAliasedObj::toString()
  {
     string rt = "ScalarAliasedObj @ " + StringUtility::numberToString(this)+ " "+ AliasedObj::toString();
     return rt;
  }
   string LabeledAggregateAliasedObj::toString()
  {
     string rt = "LabeledAggregateAliasedObj @ " + StringUtility::numberToString(this)+ " "+ AliasedObj::toString();
     return rt;
  }
    string ArrayAliasedObj::toString()
  {
     string rt = "ArrayAliasedObj @ " + StringUtility::numberToString(this)+ " "+ AliasedObj::toString();
     return rt;
  }
    string PointerAliasedObj::toString()
  {
     string rt = "PointerAliasedObj @ " + StringUtility::numberToString(this)+ " "+ AliasedObj::toString();
     return rt;
  }

/*
  bool operator == ( ScalarAliasedObj & o1, ObjSet& o2) 
  {
    return dynamic_cast<ObjSet>
  }
*/

 // A map to store aliased obj set
 // This can provide quick lookup for existing aliased objset to avoid duplicated creation
  map<SgType*, ObjSet*> aliased_objset_map; 

   void dump_aliased_objset_map ()
   {
     cout<<"Not yet implemented."<<endl;

   }
  // builder for different objects
  ObjSet* createAliasedObjSet(SgType*t)  // One object per type, Type based alias analysis
  {
    bool assert_flag = true; 
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
        assert_flag = false;
      }  

      // update the map  only if something has been created
      if (rt != NULL) 
        aliased_objset_map[t]= rt;
    }
    else // Found one, return it directly
    {
      rt = (*iter).second; 
    }  

    if (assert_flag)
      assert (rt != NULL); // we cannot assert this since not all SgType are supported now
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
