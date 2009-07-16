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

        mt->deregisterPointer(this);
    }
}

void VariablesType::invalidatePointer()
{
    assert(pointerType);
    //cout << "Invalidated Pointer " << name << " with pointed to " << pointerTarget <<  endl ;
    pointerTarget=0;
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
        pointerTarget=0;
        return;
    }

    addr_type oldPointerTarget = pointerTarget;
    pointerTarget = newAddr;
    MemoryType * oldMem = mm->findContainingMem(oldPointerTarget,pointerType->getByteSize());

    if(newMem)
    {
        // If a pointer points to that memory region, assume that his mem-region if of right type
        // even if actual error would only happen on deref
        newMem->accessMemWithType(newAddr-newMem->getAddress(),pointerType);

        if(oldMem != newMem)
            newMem->registerPointer(this);
    }


    // if old target was valid
    if(oldPointerTarget != 0)
    {
        assert(oldMem);  //has to be valid, because checked when set

        if(oldMem != newMem)
            oldMem->deregisterPointer(this);

        // old and new address is valid
        if( newMem && doChecks )
        {
            RsType * newTypeChunk = newMem->getTypeAt(newAddr-newMem->getAddress(),pointerType->getByteSize());
            RsType * oldTypeChunk = oldMem->getTypeAt(oldPointerTarget - oldMem->getAddress(),pointerType->getByteSize());

            if(oldTypeChunk == newTypeChunk && oldMem ==newMem)
                return; //pointer just changed offset in an array

            if(newAddr != 0) // memory region changed (may be error)
            {
                int oldOffset = oldPointerTarget - oldMem->getAddress();
                int newOffset = newAddr-newMem->getAddress();

                stringstream ss;
                ss << "A pointer changed the memory area (array or variable) which it points to (may be an error)" << endl << endl;

                ss << "Pointer:     " << *this << endl << endl;
                ss << "Old Target:  " <<  oldTypeChunk->getName()<< "Offset(" << oldOffset << ") in this Mem-Region:" <<  endl
                                      << *oldMem << endl;

                ss << "New Target: " << newTypeChunk->getName() << " Offset:" << newOffset << ") in this Mem-Region:" <<  endl
                                      << *oldMem << endl;

                rs->violationHandler(RuntimeViolation::POINTER_CHANGED_MEMAREA,ss.str());
            }
        }
    }


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

    if(pointerType)
        os << " points to " << pointerType->getName() << " at address 0x" << hex << setw(6) << setfill('0') << pointerTarget;
}


ostream& operator<< (ostream &os, const VariablesType & m)
{
    m.print(os);
}

