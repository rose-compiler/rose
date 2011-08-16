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

  size_t ObjSet::memSize()
  {
    cerr<<"Error. Direct call to base class's memSize() is not allowed."<<endl;
    assert (false);
    return 0;
  }

  bool ObjSet:: operator == (const ObjSet& o2)
  {
    cerr<<"Error. Direct call to base class's operator == () is not allowed."<<endl;
    assert (false);
    return false;  
  }

  bool ObjSet::operator < ( const ObjSet& o2)
  {
    cerr<<"Error. Direct call to base class's operator <() is not allowed."<<endl;
    assert (false);
    return false;  
  }

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

}
