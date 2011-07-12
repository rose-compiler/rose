#include "VariablesType.h"
#include "CppRuntimeSystem.h"


#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

VariablesType::VariablesType( Address address_,
                              const std::string & name_,
                              const std::string & mangledName_,
                              const RsType* type_
                            )
: address(address_), name(name_), mangledName(mangledName_), type(type_)
{}


const MemoryType*
VariablesType::getAllocation() const
{
    const MemoryManager* mm = rtedRTS(this)->getMemManager();
    const MemoryType*    mt = mm->getMemoryType(address);

    // assert that in this chunk only this variable is stored
    assert(mt && mt->getSize() == type->getByteSize());

    return mt;
}


size_t  VariablesType::getSize() const
{
    return type->getByteSize();
}


const PointerInfo*
VariablesType::getPointerInfo() const
{
    const PointerManager*          pm = rtedRTS(this)->getPointerManager();
    PointerManager::PointerSetIter it = pm->sourceRegionIter(address);

    if (  (it == pm->getPointerSet().end())
       || ((*it)->getSourceAddress() != address)
       )
    {
       return NULL;
    }

    return *it;
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
