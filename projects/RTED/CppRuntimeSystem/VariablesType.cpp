#include "VariablesType.h"
#include "CppRuntimeSystem.h"


#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

VariablesType::VariablesType( Address address_,
                              const std::string & name_,
                              const std::string & mangledName_,
                              RsType * type_
                            )
: address(address_), name(name_), mangledName(mangledName_), type(type_)
{}


VariablesType::~VariablesType()
{
    RuntimeSystem::instance()->freeMemory(address, akStack);
}

MemoryType * VariablesType::getAllocation() const
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
    MemoryType *    mt = mm->getMemoryType(address);

    //assert that in this chunk only this variable is stored
    assert(mt && mt->getSize() == type->getByteSize());

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
