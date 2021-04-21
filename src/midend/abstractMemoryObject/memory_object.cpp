#include "memory_object.h"

using namespace std;

namespace  AbstractMemoryObject
{
  // There should be no implementation code since only abstractions are provided
  std::set<SgType*> ObjSet::getType() 
  {
    std::set<SgType*> rt;
    cerr<<"Error. Direct call to base class's getType() is not allowed."<<endl;
    ROSE_ABORT ();
    return rt;
  };

  size_t  ObjSet::objCount()
  {
    cerr<<"Error. Direct call to base class's objCount() is not allowed."<<endl;
    ROSE_ABORT ();
    return 0;
  }

  ObjSet::~ObjSet()
  {}

  bool ObjSet::maySet()
  {
    cerr<<"Error. Direct call to base class's maySet() is not allowed."<<endl;
    ROSE_ABORT ();
    return false;  
  }
  bool ObjSet::mustSet()
  {
    cerr<<"Error. Direct call to base class's mustSet() is not allowed."<<endl;
    ROSE_ABORT ();
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
    ROSE_ABORT ();
  }

  bool ObjSet::mayEqual(const ObjSet& o2) const
  {
    cerr<<"Error. Direct call to base class's mayEqual () is not allowed."<<endl;
    ROSE_ABORT ();
  }

  bool ObjSet:: mustEqual (const ObjSet& o2) const
  {
    cerr<<"Error. Direct call to base class's mustEqual () is not allowed."<<endl;
    ROSE_ABORT ();
  }


  bool ObjSet::operator < ( const ObjSet& o2) const
  {
    cerr<<"Error. Direct call to base class's operator <() is not allowed."<<endl;
    ROSE_ABORT ();
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
/* Greg's desire output: 
  ostringstream out;
  out<<"line1\n"<<
  indent<<"line2\n"<<
  indent<<"lineLast";
  return out.str(); 
  // example use of .str()
  out << indent << "obj="<<obj.str(indent+"    ")<<" obj2="<<obj2.str(indent+"    ")<<"\n";
 * */
  std::string ObjSet::toString()
  {
    cerr<<"Error. Direct call to base class's toString() is not allowed."<<endl;
    ROSE_ABORT ();
  }
  std::vector<LabeledAggregateField*> LabeledAggregate::getElements() const 
  {
    std::vector<LabeledAggregateField*> rt;
    cerr<<"Error. Direct call to base class's getElements() is not allowed."<<endl;
    ROSE_ABORT ();
  }

  size_t LabeledAggregate::fieldCount()
  {
    cerr<<"Error. Direct call to base class's filedCount() is not allowed."<<endl;
    ROSE_ABORT ();
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
    ROSE_ABORT ();
   }

   ObjSet * LabeledAggregateField::getField()
   {

    cerr<<"Error. Direct call to base class's getField() is not allowed."<<endl;
    ROSE_ABORT ();
   }
   size_t LabeledAggregateField::getIndex()
   {

     cerr<<"Error. Direct call to base class's getIndex() is not allowed."<<endl;
     ROSE_ABORT ();
   }
   std::string LabeledAggregateField::toString()
   {
     
    cerr<<"Error. Direct call to base class's toString() is not allowed."<<endl;
    ROSE_ABORT ();
   }

   void LabeledAggregateField::setField(ObjSet* f)
   {
     cerr<<"Error. Direct call to base class's setField() is not allowed."<<endl;
     ROSE_ABORT ();
   }

   void LabeledAggregateField::setParent(LabeledAggregate* p)
   {
    cerr<<"Error. Direct call to base class's setParent() is not allowed."<<endl;
    ROSE_ABORT ();
   }

   LabeledAggregate* LabeledAggregateField::getParent()
   {
     cerr<<"Error. Direct call to base class's getParent() is not allowed."<<endl;
     ROSE_ABORT ();
   }
   std::string IndexVector::toString()
   {
    cerr<<"Error. Direct call to base class (IndexVector)'s toString() is not allowed."<<endl;
    ROSE_ABORT ();
   }

   bool IndexVector::operator ==( const IndexVector & other) const
   {
    cerr<<"Error. Direct call to base class (IndexVector)'s operator==() is not allowed."<<endl;
    ROSE_ABORT ();
   }
   bool IndexVector::mayEqual( const IndexVector & other) const
   {
    cerr<<"Error. Direct call to base class (IndexVector)'s mayEqual() is not allowed."<<endl;
    ROSE_ABORT ();
   }
   bool IndexVector::mustEqual( const IndexVector & other) const
   {
    cerr<<"Error. Direct call to base class (IndexVector)'s mustEqual() is not allowed."<<endl;
    ROSE_ABORT ();
   }



   bool IndexVector::operator !=( const IndexVector & other) const
   {
    cerr<<"Error. Direct call to base class (IndexVector)'s operator!=() is not allowed."<<endl;
    ROSE_ABORT ();
   }


   ObjSet* Array::getElements()
   {
     cerr<<"Error. Direct call to base class (Array)'s getElements() is not allowed."<<endl;
     ROSE_ABORT ();
   }

   ObjSet* Array::getElements(IndexVector* ai)
   {
     cerr<<"Error. Direct call to base class (Array)'s getElements(IndexVector* ai) is not allowed."<<endl;
     ROSE_ABORT ();
   }

   size_t Array::getNumDims()
   {
     cerr<<"Error. Direct call to base class (Array)'s getNumDims( ) is not allowed."<<endl;
     ROSE_ABORT ();
   }

   ObjSet * Array::getDereference()
   {
     cerr<<"Error. Direct call to base class (Array)'s getDereference( ) is not allowed."<<endl;
     ROSE_ABORT ();
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
   ObjSet * Pointer::getDereference()
   {
     cerr<<"Error. Direct call to base class (Pointer)'s getDereference( ) is not allowed."<<endl;
     ROSE_ABORT ();
   }

   bool Pointer::equalPoints(Pointer & that)
   {
     cerr<<"Error. Direct call to base class (Pointer)'s equalPoints( ) is not allowed."<<endl;
     ROSE_ABORT ();
   }



}
