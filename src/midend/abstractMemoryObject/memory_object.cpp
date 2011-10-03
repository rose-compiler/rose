#include "memory_object.h"

using namespace std;

namespace  AbstractMemoryObject
{
  // There should be no implementation code since only abstractions are provided
  std::set<SgType*> ObjSet::getType()  
  {
    std::set<SgType*> rt;
    cerr<<"Error. Direct call to base class's getType() is not allowed."<<endl;
    assert (false);
    return rt;
  };

  size_t  ObjSet::objCount()
  {
    cerr<<"Error. Direct call to base class's objCount() is not allowed."<<endl;
    assert (false);
    return 0;
  }

  ObjSet::~ObjSet()
  {}

  bool ObjSet::maySet()
  {
    cerr<<"Error. Direct call to base class's maySet() is not allowed."<<endl;
    assert (false);
    return false;  
  }
  bool ObjSet::mustSet()
  {
    cerr<<"Error. Direct call to base class's mustSet() is not allowed."<<endl;
    assert (false);
    return false;  
  }

/*
  size_t ObjSet::memSize()
  {
    cerr<<"Error. Direct call to base class's memSize() is not allowed."<<endl;
    assert (false);
    return 0;
  }
*/

  bool ObjSet:: operator == (const ObjSet& o2) const
  {
    cerr<<"Error. Direct call to base class's operator == () is not allowed."<<endl;
    assert (false);
    return false;  
  }

  bool ObjSet::operator < ( const ObjSet& o2) const
  {
    cerr<<"Error. Direct call to base class's operator <() is not allowed."<<endl;
    assert (false);
    return false;  
  }

/*
  bool ObjSet::operator <= ( const ObjSet& o2)
  {
    cerr<<"Error. Direct call to base class's operator <=() is not allowed."<<endl;
    assert (false);
    return false;  
  }

  bool ObjSet::operator > ( const ObjSet& o2)
  {
    cerr<<"Error. Direct call to base class's operator >() is not allowed."<<endl;
    assert (false);
    return false;  
  }

  bool ObjSet::operator >= ( const ObjSet& o2) 
  {
    cerr<<"Error. Direct call to base class's operator >=() is not allowed."<<endl;
    assert (false);
    return false;  
  }
*/
  std::string ObjSet::toString()
  {
    cerr<<"Error. Direct call to base class's toString() is not allowed."<<endl;
    assert (false);
    return "";  
  }
  std::vector<LabeledAggregateField*> LabeledAggregate::getElements() const 
  {
    std::vector<LabeledAggregateField*> rt;
    cerr<<"Error. Direct call to base class's getElements() is not allowed."<<endl;
    assert (false);
    return rt;
  }

  size_t LabeledAggregate::fieldCount()
  {
    cerr<<"Error. Direct call to base class's filedCount() is not allowed."<<endl;
    assert (false);
    return 0;
  }

/*
  bool LabeledAggregate:: operator == (const LabeledAggregate& o2) const
  {
    cerr<<"Error. Direct call to base class's operator == () is not allowed."<<endl;
    assert (false);
    return false;  
  }

  bool LabeledAggregate::operator < ( const LabeledAggregate& o2) const
  {
    cerr<<"Error. Direct call to base class's operator <() is not allowed."<<endl;
    assert (false);
    return false;  
  }
*/ 
   std::string LabeledAggregateField::getName()
   {
    cerr<<"Error. Direct call to base class's getName() is not allowed."<<endl;
    assert (false);
    return "";  
   }

   ObjSet * LabeledAggregateField::getField()
   {

    cerr<<"Error. Direct call to base class's getField() is not allowed."<<endl;
    assert (false);
    return NULL;  
   }
   size_t LabeledAggregateField::getIndex()
   {

     cerr<<"Error. Direct call to base class's getIndex() is not allowed."<<endl;
     assert (false);
     return 0;
   }
   std::string LabeledAggregateField::toString()
   {
     
    cerr<<"Error. Direct call to base class's toString() is not allowed."<<endl;
    assert (false);
    return "";  
   }

   void LabeledAggregateField::setField(ObjSet* f)
   {
     cerr<<"Error. Direct call to base class's setField() is not allowed."<<endl;
     assert (false);
   }

   void LabeledAggregateField::setParent(LabeledAggregate* p)
   {
    cerr<<"Error. Direct call to base class's setParent() is not allowed."<<endl;
    assert (false);
   }

   LabeledAggregate* LabeledAggregateField::getParent()
   {
     cerr<<"Error. Direct call to base class's getParent() is not allowed."<<endl;
     assert (false);
     return NULL;  
   }
   std::string IndexVector::toString()
   {
    cerr<<"Error. Direct call to base class (IndexVector)'s toString() is not allowed."<<endl;
    assert (false);
    return "";  
   }
   bool IndexVector::operator ==( const IndexVector & other) const
   {
    cerr<<"Error. Direct call to base class (IndexVector)'s operator==() is not allowed."<<endl;
    assert (false);
    return false;  
   }


   ObjSet* Array::getElements()
   {
     cerr<<"Error. Direct call to base class (Array)'s getElements() is not allowed."<<endl;
     assert (false);
     return NULL;  
   }

   ObjSet* Array::getElements(IndexVector* ai)
   {
     cerr<<"Error. Direct call to base class (Array)'s getElements(IndexVector* ai) is not allowed."<<endl;
     assert (false);
     return NULL;  
   }

   size_t Array::getNumDims()
   {
     cerr<<"Error. Direct call to base class (Array)'s getNumDims( ) is not allowed."<<endl;
     assert (false);
    return 0;
   }

/*
  bool Array:: operator == (const ObjSet & o2) const
  {
    cerr<<"Error. Direct call to base class (Array)'s operator == () is not allowed."<<endl;
    assert (false);
    return false;  
  }

  bool Array:: operator  (const ObjSet & o2) const
  {
    cerr<<"Error. Direct call to base class (Array)'s operator == () is not allowed."<<endl;
    assert (false);
    return false;  
  }
*/


}
