#include "MemoryManager.h"


#include <sstream>
#include <iostream>
#include <iomanip>


#include "CppRuntimeSystem.h"

using namespace std;

// -----------------------    MemoryType  --------------------------------------


MemoryType::MemoryType(addr_type _addr, size_t _size, const SourcePosition & _pos, bool _onStack)
    : startAddress(_addr), size(_size), allocPos(_pos), onStack(_onStack)
{
    // not memory has been initialized by default
    initialized.assign(size,false);
}

MemoryType::MemoryType(addr_type _addr, size_t _size, bool _onStack,
                       const std::string & file, int line1, int line2)
    : startAddress(_addr),
      size(_size),
      allocPos(file,line1,line2),
      onStack(_onStack)
{
}

MemoryType::MemoryType(addr_type addr, bool _onStack)
    : startAddress(addr), size(0), allocPos("unknown",-1,-1), onStack(_onStack)
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

bool MemoryType::overlapsMemArea(addr_type queryAddr, size_t querySize)
{
    return ( containsAddress(queryAddr) ||
             containsAddress(queryAddr+querySize-1) );
}

bool MemoryType::operator< (const MemoryType & other) const
{
    return (startAddress < other.startAddress);
}


bool MemoryType::isInitialized(int offsetFrom, int offsetTo) const
{
    assert(offsetFrom<offsetTo);
    assert(offsetFrom < size && offsetTo <= size);

    for(int i= offsetFrom; i<offsetTo; i++)
        if(! initialized[i])
            return false;

    return true;
}

void MemoryType::initialize(int offsetFrom, int offsetTo)
{
    assert(offsetFrom<offsetTo);
    assert(offsetFrom < size && offsetTo <= size);

    for(int i=offsetFrom; i<offsetTo; i++)
        initialized[i]=true;
}


void MemoryType::registerPointer(VariablesType * var)
{
    pointerSet.insert(var);
}

void MemoryType::deregisterPointer(VariablesType * var, bool doChecks)
{
    size_t erasedElements= pointerSet.erase(var);
    if(erasedElements==0)
    {
        cerr << "Warning: Tried to deregister a pointer which has not been registered" << endl;
    }

    if(pointerSet.size()==0 && doChecks)
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        stringstream ss;
        ss << "No pointer to allocated memory region: " << endl;
        ss << *this << endl << " because this pointer has changed: " <<endl;
        ss << *var << endl;

        rs->violationHandler(RuntimeViolation::MEM_WITHOUT_POINTER, ss.str());
    }
}


void MemoryType::setTypeInfo(addr_type offset, RsType * type)
{
    TypeInfoMap::iterator itLower = findPossibleTypeMatch(offset);
    TypeInfoMap::iterator itUpper = typeInfo.lower_bound(offset);

    stringstream ss;
    ss << "Tried to access the same memory region with different types" << endl;

    RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS,ss.str());


    int newTiStart = offset;
    int newTiEnd = offset + type->getByteSize();


    vio.descStream() << "When trying to access (" << newTiStart << "," << newTiEnd << ") " <<
                        "as type " << type->getName() << endl;


    RsType * containingType = NULL;
    int      containingOffset =0;
    int      containingStart  = -1;

    if(itLower != typeInfo.end())
    {
        int lowerStart = itLower->first;
        int lowerEnd = lowerStart + itLower->second->getByteSize();
        if(newTiStart < lowerEnd  ) // Check for overlapping with previous
        {
            //Test if oldInfo contains the whole newInfo
            if(newTiEnd <= lowerEnd)
            {
                containingType = itLower->second;
                containingStart = lowerStart;
            }
            else
            {
                vio.descStream() << "Previously registered  (" << lowerStart << "," << lowerEnd << ") " <<
                                    "as " << itLower->second->getName() << endl;
                RuntimeSystem::instance()->violationHandler(vio);
                return;
            }
        }
    }

    if(itUpper != typeInfo.end())
    {
        int upperStart = itUpper->first;
        int upperEnd = upperStart + itUpper->second->getByteSize();
        if(upperStart < newTiEnd) // Check for overlapping with next chunk
        {
            //Test if oldInfo contains the whole newInfo
            if(newTiStart >= upperStart)
            {
                containingType = itUpper->second;
                containingStart = upperStart;
            }
            else
            {
                vio.descStream() << "Previously registered  (" << upperStart << "," << upperEnd << ") " <<
                                    "as " << itUpper->second->getName() << endl;

                RuntimeSystem::instance()->violationHandler(vio);
                return;
            }
        }
    }

    if(containingType)
    {
        RsType * sub = containingType->getSubtypeRecursive(offset-containingStart,type->getByteSize());
        if(sub != type )
        {
            vio.descStream() << "Previously registered Type overlapps in a nonconsistent way:" << endl
                             << "Containing Type " << containingType->getName()
                             << " (" << containingStart << "," << containingStart + containingType->getByteSize() << ")" << endl;
            RuntimeSystem::instance()->violationHandler(vio);
        }

        return;
    }

    typeInfo.insert(make_pair<int,RsType*>(offset,type));
}

RsType * MemoryType::getTypeInfo(addr_type searchOffset)
{
    TypeInfoMap::iterator it = findPossibleTypeMatch(searchOffset);

    if (it == typeInfo.end() )
        return NULL;

    addr_type curTypeOffset = it->first;
    RsType *  curType       = it->second;
    int off = searchOffset - curTypeOffset;

    // Check if a type registration has been found
    if ( off < 0 || off >= curType->getByteSize() )
        return NULL;

    RsType *  refinedType;
    addr_type refinedOffset;
    RsType::getTypeAt<RsArrayType>(curType,off,refinedType,refinedOffset);

    if(!refinedType) //search offset was in padding mem
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        stringstream ss;
        ss << "Tried to access padded memory-area" << endl;
        ss << "Allocated type:" << curType->getName() <<
              " at offset " << off << endl;

        rs->violationHandler(RuntimeViolation::INVALID_TYPE_ACCESS, ss.str());
    }

    return refinedType;
}

MemoryType::TypeInfoMap::iterator MemoryType::findPossibleTypeMatch(addr_type offset)
{
    if(typeInfo.size() ==0 )
        return typeInfo.end();

    TypeInfoMap::iterator it = typeInfo.lower_bound(offset);

    // case 0: we are at the end and map.size>0, so return last element
    if(it == typeInfo.end())
    {
        --it;
        return it;
    }

    //case 1: offset is startAdress of a chunk, exact match
    if( it->first == offset )
    {
        return it;
    }

    //case 2: offset is greater than a type's startaddress
    //        try to get next smaller one
    assert( it->first > offset);

    if( it == typeInfo.begin())
        return typeInfo.end();

    --it;

    return it;
}



// extra print function because operator<< cannot be member-> no access to privates
void MemoryType::print(ostream & os) const
{
    std::string init = "Part. initialized";
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
        init = "Initialized      ";
    if(noneInit)
        init = "Not initialized  ";



    os << "0x" << setfill('0')<< setw(6) << hex << startAddress
       << " Size " << dec << size <<  "\t" << init << "\tAllocated at " << allocPos << endl ;

    if(pointerSet.size() > 0)
    {
        os << "\tPointer: ";
        for(set<VariablesType*>::const_iterator i = pointerSet.begin(); i!= pointerSet.end(); ++i)
            os << "\t" << (*i)->getName() << " ";
    }

    if(typeInfo.size() > 0)
    {
        os << "\t" << "Type Info" << endl;
        for(TypeInfoMap::const_iterator i = typeInfo.begin(); i != typeInfo.end(); ++i)
            os << "\t" << i->first << "\t" << i->second->getName() << endl;
    }

}

ostream& operator<< (ostream &os, const MemoryType & m)
{
    m.print(os);
    return os;
}




// -----------------------    MemoryManager  --------------------------------------

MemoryManager::MemoryManager()
{
}

MemoryManager::~MemoryManager()
{
    checkForNonFreedMem();

    // free all the allocation-types
    for(set<MemoryType*>::iterator i = mem.begin(); i != mem.end(); ++i)
        delete *i;
}


MemoryType * MemoryManager::findPossibleMemMatch(addr_type addr)
{
   if(mem.size()==0)
        return NULL;

    // search the memory chunk, we get the chunk which has equal or greater startAdress
    MemoryType compareObject (addr);
    MemoryTypeSet::iterator it = mem.lower_bound(&compareObject);

    // case 0: we are at the end and map.size>0, so return last element
    if(it == mem.end())
    {
        --it;
        return (*it);
    }

    //case 1: addr is startAdress of a chunk, exact match
    if((*it)->getAddress() == addr )
    {
        return (*it);
    }

    //case 2: addr is greater than a chunk startadress
    //        try to get next smaller one
    assert( (*it)->getAddress() > addr);

    if( it == mem.begin())
        return NULL;

    --it;

    return (*it);
}

MemoryType * MemoryManager::findContainingMem(addr_type addr, size_t size)
{
    MemoryType * res = findPossibleMemMatch(addr);
    if(!res)
        return NULL;

    return res->containsMemArea(addr,size) ?  res  : NULL;
}

MemoryType * MemoryManager::findOverlappingMem(addr_type addr, size_t size)
{
    MemoryType * res = findPossibleMemMatch(addr);
    if(!res)
        return NULL;

    return res->overlapsMemArea(addr,size) ?  res  : NULL;
}



void MemoryManager::allocateMemory(MemoryType * alloc)
{
    RuntimeSystem * rs = RuntimeSystem::instance();
    if(alloc->getSize()==0 )
    {
        rs->violationHandler(RuntimeViolation::EMPTY_ALLOCATION,"Tried to call malloc/new with size 0\n");
        return;
    }


    MemoryType * m = findOverlappingMem(alloc->getAddress(), alloc->getSize());
    if(m)
    {
        // the start address of new chunk lies in already allocated area
        rs->violationHandler(RuntimeViolation::DOUBLE_ALLOCATION);
        return;
    }

    mem.insert(alloc);
}


void MemoryManager::freeMemory(addr_type addr, bool onStack)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    MemoryType * m = findContainingMem(addr,1);

    // free on unknown address
    if(m == NULL)
    {
        stringstream desc;
        desc << "Free was called with address " << "0x" << hex << addr << endl;
        desc << "Allocated Memory Regions:" << endl;
        print(desc);
        rs->violationHandler(RuntimeViolation::INVALID_FREE,desc.str());
        return;
    }

    // free inside an allocated memory block
    if(m->getAddress() != addr)
    {
        stringstream desc;
        desc << "Free was called with an address inside of allocated block (Offset:"
             << "0x" << hex << addr - m->getAddress() <<")" << endl;
        desc << "Allocated Block: " << *m << endl;

        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    }

    // free stack memory explicitly (i.e. not via exitScope)
    if(m->isOnStack() && !onStack)
    {
        stringstream desc;
        desc << "Stack memory was explicitly freed (0x" 
             << *m << endl;

        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    }

    // successful free, erase allocation info from map
    mem.erase(m);
}



void MemoryManager::checkRead(addr_type addr, size_t size)
{
    RuntimeSystem * rs = RuntimeSystem::instance();


    MemoryType * mt = findContainingMem(addr,size);
    if(!mt)
    {
        stringstream desc;
        desc    << "Trying to read from non-allocated MemoryRegion "
                << "(Address " << "0x" << hex << addr 
                <<" of size " << dec << size << ")" << endl;

        MemoryType * possMatch = findPossibleMemMatch(addr);
        if(possMatch)
            desc << "Did you try to read this region:" << endl << *possMatch << endl;

        rs->violationHandler(RuntimeViolation::INVALID_READ,desc.str());
        return;
    }

    assert(addr >= mt->getAddress());
    int from = addr - mt->getAddress();

    if(! mt->isInitialized(from, from +size) )
    {
        stringstream desc;
        desc    << "Trying to read from uninitialized MemoryRegion "
                << "(Address " << "0x" << hex << addr 
                <<" of size " << dec << size << ")" << endl;
        rs->violationHandler(RuntimeViolation::INVALID_READ,desc.str());
    }
}

void MemoryManager::checkWrite(addr_type addr, size_t size)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    MemoryType * mt = findContainingMem(addr,size);
    if(!mt)
    {
        stringstream desc;
        desc    << "Trying to write to non-allocated MemoryRegion "
                << "(Address " << "0x" << hex << addr 
                <<" of size " << dec << size << ")" << endl;

        MemoryType * possMatch = findPossibleMemMatch(addr);
        if(possMatch)
            desc << "Did you try to write to this region:" << endl << *possMatch << endl;

        rs->violationHandler(RuntimeViolation::INVALID_WRITE,desc.str());
        return;
    }

    assert(addr >= mt->getAddress());
    int from = addr - mt->getAddress();
    mt->initialize(from,from + size);
}


void MemoryManager::checkForNonFreedMem() const
{
    stringstream desc;
    if(mem.size() !=0)
    {
        desc << "Program terminated and the following allocations were not freed:" << endl;
        print(desc);
        RuntimeSystem::instance()->violationHandler(RuntimeViolation::MEMORY_LEAK,desc.str());
    }
}


MemoryType * MemoryManager::getMemoryType(addr_type addr)
{
    MemoryType * possibleMatch = findPossibleMemMatch(addr);

    return possibleMatch->getAddress() == addr ? possibleMatch : NULL;
}


void MemoryManager::print(ostream & os) const
{
    if(mem.size() ==0 )
    {
        os << "Memory Manager: No allocations" << endl;
        return;
    }

    os << endl;
    os << "------------------------------- Memory Manager Status -----------------------------" << endl << endl;
    for(set<MemoryType*>::iterator i = mem.begin(); i != mem.end(); ++i)
        os << **i << endl;

    os << "-----------------------------------------------------------------------------------" << endl;
    os << endl;
}

ostream& operator<< (ostream &os, const MemoryManager & m)
{
    m.print(os);
}

