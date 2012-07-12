#include <iostream>
#include <iomanip>
#include <sstream>

#include "VariablesType.h"
#include "CppRuntimeSystem.h"

const MemoryType*
VariablesType::getAllocation() const
{
    const MemoryManager& mm = rtedRTS(this).getMemManager();
    const MemoryType*    mt = mm.getMemoryType(address);

    // assert that in this chunk only this variable is stored
    assert(mt && mt->getSize() == type->getByteSize());

    return mt;
}


const PointerInfo*
VariablesType::getPointerInfo() const
{
    const PointerManager&          pm = rtedRTS(this).getPointerManager();
    PointerManager::PointerSetIter it = pm.sourceRegionIter(address);

    if (  (it == pm.getPointerSet().end())
       || ((*it)->getSourceAddress() != address)
       )
    {
       return NULL;
    }

    return *it;
}



void VariablesType::print(std::ostream& os) const
{
    os << std::setw(6) << std::setfill('0') << address
       << "\t" << name << "(" << mangledName <<")"
       << " Type: " << type->getName()  ;
}


std::ostream& operator<< (std::ostream& os, const VariablesType & m)
{
    m.print(os);
    return os;
}
