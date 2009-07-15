#include "VariablesType.h"
#include "CppRuntimeSystem.h"


#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


VariablesType::VariablesType(const std::string & name_,
                             const std::string & mangledName_,
                             const std::string & typeStr_,
                             addr_type address_,
                             RsType * pointerType_) :
    name(name_),
    mangledName(mangledName_),
    address(address_),
    pointerTarget(0),
    pointerType(pointerType_)
{
    TypeSystem * ts = RuntimeSystem::instance()->getTypeSystem();
    type = ts->getTypeInfo(typeStr_);
    assert(type); //type of this variable not yet registered
}

VariablesType::VariablesType(const std::string & name_,
                             const std::string & mangledName_,
                             RsType * type_,
                             addr_type address_,
                             RsType * pointerType_) :
    name(name_),
    mangledName(mangledName_),
    type(type_),
    address(address_),
    pointerTarget(0),
    pointerType(pointerType_)
{
    assert(type);
}




VariablesType::~VariablesType()
{
    RuntimeSystem::instance()->freeMemory(address, true);

    if(pointerTarget != 0)
    {
        MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
        MemoryType * mt =mm->findContainingMem(pointerTarget,1);
        assert(mt);

        mt->deregisterPointer(this,true);
    }
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



void VariablesType::setPointerTarget(addr_type newAddr, bool doChecks)
{
    assert(pointerType); //this variable was not registered as a pointer

    if(pointerTarget==0)//inital assignment -> no checks possible
        doChecks=false;


    // Check if newAddr points to valid mem-region
    RuntimeSystem * rs = RuntimeSystem::instance();
    MemoryManager * mm = rs->getMemManager();

    MemoryType * newMem = mm->findContainingMem(newAddr,pointerType->getByteSize() );

    if(!newMem && newAddr != 0) //new address is invalid
    {
        stringstream ss;
        ss << "Tried to assign non allocated address 0x" << hex << newAddr;
        ss << " to pointer " << name;
        rs->violationHandler(RuntimeViolation::INVALID_PTR_ASSIGN,ss.str());
        return;
    }

    if(pointerTarget!= 0)
    {
        MemoryType * oldMem = mm->findContainingMem(pointerTarget,pointerType->getByteSize());
        if(oldMem == newMem) //pointer just changed offset
            return;

        if(doChecks && newAddr != 0) // memory region changed (may be error)
        {
            stringstream ss;
            ss << "A pointer changed the memory area which it points to (may be an error)" << endl;
            ss << "Pointer:     " << *this << endl;
            ss << "Old MemArea: " << *oldMem << endl;
            ss << "New MemArea: " << *newMem << endl;
            rs->violationHandler(RuntimeViolation::POINTER_CHANGED_MEMAREA);
        }

        // deregister old target
        oldMem->deregisterPointer(this,doChecks);
    }

    if(newAddr!= 0)
        newMem->registerPointer(this);

    pointerTarget = newAddr;
}


MemoryType * VariablesType::getTargetAllocation() const
{
    if(pointerTarget==0)
        return NULL;

    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
    MemoryType *    mt = mm->getMemoryType(pointerTarget);

    //should always point to a valid mem-region
    //if mem has been invalidated the pointer should have been set to null
    //internally
    assert(mt);

    return mt;
}

size_t  VariablesType::getSize() const
{
    return type->getByteSize();
}

void VariablesType::print(ostream & os) const
{
    os << "0x" << hex <<setw(6) << setfill('0') << address << "\t" << name << "(" << mangledName <<")" << " Type: " << type->getName()  ;
}


ostream& operator<< (ostream &os, const VariablesType & m)
{
    m.print(os);
}

