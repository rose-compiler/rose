#include "VariablesType.h"
#include "CppRuntimeSystem.h"


#include <iostream>
#include <iomanip>

using namespace std;


VariablesType::VariablesType(const std::string & _name,
                             const std::string & _mangledName,
                             const std::string & _typeStr,
                             addr_type _address) :
    name(_name),
    mangledName(_mangledName),
    type(_typeStr),
    address(_address)
{

}

VariablesType::~VariablesType()
{
    RuntimeSystem::instance()->freeMemory(address);
}

MemoryType * VariablesType::getAllocation() const
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();

    MemoryType * mt =mm->getMemoryType(address);
    assert(mt);
    //FIXME
    //assert(mtt->getSize() ==??? )
    return mt;
}



void VariablesType::print(ostream & os) const
{
    os << "0x" << hex <<setw(6) << setfill('0') << address << "\t" << name << "(" << mangledName <<")" << " Type: " << type  ;
}


ostream& operator<< (ostream &os, const VariablesType & m)
{
    m.print(os);
}

