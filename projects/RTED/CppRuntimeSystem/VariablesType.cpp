#include "VariablesType.h"
#include "CppRuntimeSystem.h"


#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

VariablesType::VariablesType( const std::string & name_,
                              const std::string & mangledName_,
                              RsType * type_,
                              Address address_
                            )
: name(name_), mangledName(mangledName_), type(type_), address(address_)
{
  assert(type != NULL);

  RsClassType*   class_type = dynamic_cast< RsClassType* >( type );

  // Variables are allocated statically. However, in UPC they can be shared
  //   if allocated on the file scope. Thus, the locality holds only for
  //   non UPC types (e.g., C++ classes).
  assert(class_type == NULL || rted_isLocal(address));

  RuntimeSystem* rs = RuntimeSystem::instance();
  const bool     isCtorCall = (  class_type != NULL
                              && rs->getMemManager()->getMemoryType(address) != NULL
                              );

  // When we create classes, the memory might be allocated in the
  // constructor.  In these cases, it's fine to call createvar with
  // existing memory
  if (!isCtorCall)
  {
    rs->createMemory(address_, type->getByteSize(), akStack, type);
  }
}


VariablesType::~VariablesType()
{
    RuntimeSystem::instance()->freeMemory(address, akStack);
}

MemoryType * VariablesType::getAllocation() const
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
    MemoryType *    mt = mm->getMemoryType(address);

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
    PointerManager *               pm = RuntimeSystem::instance()->getPointerManager();
    PointerManager::PointerSetIter it = pm->sourceRegionIter(address);

    if (it == pm->getPointerSet().end())
        return NULL;

    if ((*it)->getSourceAddress() == address)
        return *it;
    else
        return NULL;
}



void VariablesType::print(ostream & os) const
{
    os << setw(6) << setfill('0') << address
       << "\t" << name << "(" << mangledName <<")"
       << " Type: " << type->getName()  ;
}


ostream& operator<< (ostream &os, const VariablesType & m)
{
    m.print(os);
    return os;
}
