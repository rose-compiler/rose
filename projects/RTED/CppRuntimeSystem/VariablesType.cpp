#include "VariablesType.h"
#include "CppRuntimeSystem.h"


#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


VariablesType::VariablesType(const std::string & name_,
                             const std::string & mangledName_,
                             const std::string & typeStr_,
                             MemoryAddress address_) :
    name(name_),
    mangledName(mangledName_),
    address(address_)
{
    TypeSystem * ts = RuntimeSystem::instance()->getTypeSystem();
    type = ts->getTypeInfo(typeStr_);

    if(!type)
    {
        if(typeStr_ == "SgArrayType")
        {
            //TODO just until registration is done correct
            cerr << "Trying to register an array via createVariable!" << endl;
            cerr << "Use createArray instead" << endl;
            type = ts->getTypeInfo("SgPointerType");
            return;
        }
        else
            assert(false);//unknown type
    }

    RsClassType* class_type = dynamic_cast< RsClassType* >( type );
    if(     class_type != NULL
            && RuntimeSystem::instance() -> getMemManager()
                -> getMemoryType( address )) {
        // When we create classes, the memory might be allocated in the
        // constructor.  In these cases, it's fine to call createvar with
        // existing memory
        return;
    }

    RuntimeSystem::instance()->createMemory(address,type->getByteSize(),true,false,type);

}

VariablesType::VariablesType(const std::string & name_,
                             const std::string & mangledName_,
                             RsType * type_,
                             MemoryAddress address_) :
    name(name_),
    mangledName(mangledName_),
    type(type_),
    address(address_)
{
  assert(type);

  RsClassType* class_type = dynamic_cast< RsClassType* >( type );
  if(     class_type != NULL
          && RuntimeSystem::instance() -> getMemManager()
              -> getMemoryType( address )) {
      // When we create classes, the memory might be allocated in the
      // constructor.  In these cases, it's fine to call createvar with
      // existing memory
      return;
  }

  RuntimeSystem::instance()->createMemory(address,type->getByteSize(),true,false,type);
}




VariablesType::~VariablesType()
{
    RuntimeSystem::instance()->freeMemory(address, true);
}

MemoryType * VariablesType::getAllocation() const
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
    MemoryType * mt =mm->getMemoryType(address);

    assert(mt);
    //assert that in this chunk only this variable is stored
    assert(mt->getSize() == type->getByteSize());

    return mt;
}


size_t  VariablesType::getSize() const
{
    return type->getByteSize();
}


PointerInfo * VariablesType::getPointerInfo() const
{
    PointerManager * pm = RuntimeSystem::instance()->getPointerManager();
    PointerManager::PointerSetIter it = pm->sourceRegionIter(getAddress());

    if(it == pm->getPointerSet().end())
        return NULL;

    if( (*it)->getSourceAddress() == getAddress())
        return *it;
    else
        return NULL;
}



void VariablesType::print(ostream & os) const
{
    os << "0x" << hex <<setw(6) << setfill('0') << address << "\t" << name << "(" << mangledName <<")" << " Type: " << type->getName()  ;
}


ostream& operator<< (ostream &os, const VariablesType & m)
{
    m.print(os);
    return os;
}
