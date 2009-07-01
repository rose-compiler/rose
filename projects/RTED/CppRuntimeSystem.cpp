#include "CppRuntimeSystem.h"
#include <cassert>

#include <sstream>

using namespace std;

// ----------------------- SourcePosition --------------------------------------
ostream& operator<< (ostream &os, const SourcePosition & m)
{
    os << m.file << "(" << m.line1 << "," << m.line2;
    return os;
}


// -----------------------    MemoryType  --------------------------------------


MemoryType::MemoryType(addr_type _addr, size_t _size, const SourcePosition & _pos)
    : startAddress(_addr), size(_size), allocPos(_pos)
{
    // not memory has been initialized by default
    initialized.assign(size,false);
}

MemoryType::MemoryType(addr_type _addr, size_t _size,
                       const std::string & file, int line1, int line2)
    : startAddress(_addr),
      size(_size),
      allocPos(file,line1,line2)
{
}

MemoryType::MemoryType(addr_type addr)
    : startAddress(addr), size(0), allocPos("unknown",-1,-1)
{
}


bool MemoryType::containsAddress(addr_type queryAddress)
{
    return ( queryAddress >= startAddress &&
             queryAddress < startAddress+size );
}

bool MemoryType::containsMemArea(addr_type queryAddr, size_t querySize)
{
    return ( containsAddress(queryAddr) &&
             containsAddress(queryAddr+querySize-1) );
}


bool MemoryType::operator< (const MemoryType & other) const
{
    return (startAddress < other.startAddress);
}


bool MemoryType::isInitialized(int offsetFrom, int offsetTo) const
{
    assert(offsetFrom<offsetTo);

    for(int i= offsetFrom; i<offsetFrom; i++)
        if(! initialized[i])
            return false;

    return true;

}


// extra print function because operator<< cannot be member-> no access to privates
void MemoryType::print(ostream & os) const
{
    std::string init = "Partially initialized";
    bool allInit  = true;
    bool noneInit = true;
    for(vector<bool>::const_iterator it = initialized.begin();
            it != initialized.end();
            ++it)
    {
        if( *it)
            noneInit=false;
        if (! *it)
            allInit=false;
    }

    if(allInit)
        init = "Initialized";
    if(noneInit)
        init = "Not initialized";


    os << allocPos << "\t" << "at " << startAddress << " Size:" << size <<  " " << init << endl;

}

ostream& operator<< (ostream &os, const MemoryType & m)
{
    m.print(os);
    return os;
}




// -----------------------    MemoryManager  --------------------------------------

MemoryManager::~MemoryManager()
{
    checkForNonFreedMem();

    // free all the allocation-types
    for(set<MemoryType*>::iterator i = mem.begin(); i != mem.end(); ++i)
        delete *i;
}


MemoryType * MemoryManager::findMem(addr_type addr, size_t size)
{
    // search the memory chunk, we get the chunk which has equal or greater startAdress
    MemoryType compareObject (addr);
    MemoryTypeSet::iterator it = mem.lower_bound(&compareObject);

    if(it == mem.end()) //not found
        return NULL;

    //case 1: addr is startAdress of a chunk
    if((*it)->getAdress() == addr )
    {
        return  (*it)->containsMemArea(addr,size) ?  *it  : NULL;
    }

    //case 2: addr is greater than a chunk startadress
    //        try to get next smaller one
    assert( (*it)->getAdress() > addr);

    if( it == mem.begin())
        return NULL;

    --it;

    return  (*it)->containsMemArea(addr,size) ?  *it  : NULL;

}


void MemoryManager::allocateMemory(MemoryType * alloc)
{
    MemoryType * m = findMem(alloc->getAdress(), alloc->getSize());
    if(m)
    {
        // the start address of new chunk lies in already allocated area
        RuntimeSystem::instance()->violationHandler(RuntimeSystem::DOUBLE_ALLOCATION);
        return;
    }

    mem.insert(alloc);
}




void MemoryManager::checkForNonFreedMem() const
{
    stringstream desc;
    desc << "Program terminated and the following allocations were not freed:" << endl;
    print(desc);
    RuntimeSystem::instance()->violationHandler(RuntimeSystem::MEMORY_LEAK,desc.str());
}


void MemoryManager::print(ostream & os) const
{
    os << "-------------- Memory Manager Status --------------" << endl;
    for(set<MemoryType*>::iterator i = mem.begin(); i != mem.end(); ++i)
        os << **i << endl;

}

ostream& operator<< (ostream &os, const MemoryManager & m)
{
    m.print(os);
}




// -----------------------    MemoryManager  --------------------------------------

RuntimeSystem * RuntimeSystem::single = NULL;

RuntimeSystem* RuntimeSystem::instance()
{
    if(!single)
        single = new RuntimeSystem();

    return single;
}


void RuntimeSystem::violationHandler(Violation v, const string & desc)
{

    switch(v)
    {
        case DOUBLE_ALLOCATION:
            cerr << "Double Allocation fault" << endl << desc;
            abort();
            break;
        case MEMORY_LEAK:
            cerr << "Memory Leaks detected" << endl << desc;
            abort();
            break;

    }

}







