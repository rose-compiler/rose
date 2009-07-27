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

MemoryType::~MemoryType()
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



void MemoryType::accessMemWithType(addr_type offset, RsType * type)
{
    if(typeInfo.size() ==0) // no types registered yet
    {
        typeInfo.insert(make_pair<int,RsType*>(offset,type));
        return;
    }

    int newTiStart = offset;
    int newTiEnd = offset + type->getByteSize();


    RuntimeSystem * rs = RuntimeSystem::instance();
    stringstream ss;
    ss << "Tried to access the same memory region with different types" << endl;
    ss << "When trying to access (" << newTiStart << "," << newTiEnd << ") "
       << "as type " << type->getName()
       << "Memory Region Info: " << endl << *this << endl;

    RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS,ss.str());

    // Get a range of entries which overlap with [newTiStart,newTiEnd)
    TiIterPair res = getOverlappingTypeInfos(newTiStart,newTiEnd);
    TiIter itLower = res.first;
    TiIter itUpper = res.second;

    //  Case 1: New entry is overlapped by one old entry,
    //  i.e. [itLower,itUpper) contains only one entry
    TiIter incrementedLower = itLower;
    ++incrementedLower;

    if(incrementedLower == itUpper)
    {
        int oldTiStart = itLower->first;
        int oldTiEnd = oldTiStart + itLower->second->getByteSize();
        RsType * oldType = itLower->second;
        if( oldTiStart <= newTiStart && oldTiEnd >= newTiEnd )
        {
            //Check if new entry is consistent with old one
            RsType * sub =  oldType->getSubtypeRecursive(newTiStart - oldTiStart,type->getByteSize());
            if(sub != type)
            {
                vio.descStream() << "Previously registered Type completely overlaps new Type in an inconsistent way:" << endl
                                 << "Containing Type " << oldType->getName()
                                 << " (" << oldTiStart << "," << oldTiEnd << ")" << endl;

                rs->violationHandler(vio);
                return;
            }
            else
            {
                //successful, no need to add because the type is already contained in bigger one
                return;
            }
        }
    }

    // Case 2: Iterate over overlapping old types and check consistency
    for(TypeInfoMap::iterator i = itLower; i != itUpper; ++i )
    {
        RsType * curType = i->second;
        int curStart     = i->first;
        int curEnd       = curStart + curType->getByteSize();

        if(curStart < newTiStart || curEnd > newTiEnd )
        {
            vio.descStream() << "Previously registered Type overlaps new Type in an inconsistent way:" << endl
                             << "Overlapping Type " << curType->getName()
                             << " (" << curStart << "," << curEnd << ")" << endl;
            rs->violationHandler(vio);
            return;
        }

        RsType * sub =  type->getSubtypeRecursive(curStart - newTiStart,curType->getByteSize());
        if (sub != curType)
        {
            vio.descStream() << "Newly registered Type completely overlaps a previous registered Type in an inconsistent way:" << endl
                             << "Overlapping Type " << curType->getName()
                             << " (" << curStart << "," << curEnd << ")" << endl;

            rs->violationHandler(vio);
            return;
        }
    }
    //All consistent - old typeInfos are replaced by big new typeInfo (merging)
    typeInfo.erase(itLower,itUpper);

    typeInfo.insert(make_pair<int,RsType*>(offset,type));

    // if we have knowledge about the type in memory, we also need to update the
    // information about "dereferentiable" memory regions i.e. pointer
    RuntimeSystem::instance()->getPointerManager()->createPointer(startAddress+offset,type);
    return;
}


string MemoryType::getTypeAt(addr_type offset, size_t size)
{
    TiIterPair res = getOverlappingTypeInfos(offset,offset+size);
    TiIter itLower = res.first;
    TiIter itUpper = res.second;

    if(itLower== typeInfo.end())
        return "";


    // makes only sense if range contains exactly one type
    TiIter incrementedLower = itLower;
    ++incrementedLower;

    if(incrementedLower == itUpper)
    {
        assert(offset >= itLower->first);
        string typeStr;
        itLower->second->getSubtypeRecursive(offset - itLower->first,size,true,&typeStr);
        return typeStr;
    }

    return "";
}

bool MemoryType::isArrayPossible(addr_type o1, addr_type o2, RsType *t)
{
    size_t size = t->getByteSize();
    TiIterPair res = getOverlappingTypeInfos(o1,o2+size);


    for(TiIter i = res.first; i != res.second; ++i)
    {
        addr_type offset = i->first;
        RsType * curType = i->second;
        if(curType != t)
            return false;
        if(offset % size != 0)
            return false;
    }

    return true;
}


MemoryType::TiIterPair MemoryType::getOverlappingTypeInfos(addr_type from, addr_type to)
{
    // This function assumes that the typeInfo ranges do not overlap

    //cerr << "Current status";
    //RuntimeSystem::instance()->getMemManager()->print(cerr);
    //cerr << "Searching for offset " << from << ","  << to << endl;

    // catch the case that map is empty
    if(typeInfo.size() ==0 )
        return make_pair<TiIter,TiIter>(typeInfo.end(), typeInfo.end());

    // end is the iterator which points to typeInfo with offset >= to
    TiIter end = typeInfo.lower_bound(to);

    // the same for the beginning, but there we need the previous map entry
    TiIter it = typeInfo.lower_bound(from);

    if(it == typeInfo.end() ) //we are at the end and map.size>0, take last element
        --it;
    if(it->first > from && it != typeInfo.begin())
        --it;

    // range not overlapping
    if(it != end && it->first + it->second->getByteSize() <= from)
        ++it;

    //for(TiIter i = it; i != end; ++i)
    //    cerr  << "\t "<< i->first  << "\t" << i->second->getName() << endl ;


    return make_pair<TiIter,TiIter>(it, end);
}



string MemoryType::getInitString() const
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

    return init;
}

// extra print function because operator<< cannot be member-> no access to privates
void MemoryType::print(ostream & os) const
{
    os << "0x" << setfill('0')<< setw(6) << hex << startAddress
       << " Size " << dec << size <<  "\t" << getInitString() << "\tAllocated at " << allocPos << endl ;

    PointerManager * pm = RuntimeSystem::instance()->getPointerManager();
    typedef PointerManager::TargetToPointerMapIter Iter;
    Iter it =  pm->targetRegionIterBegin(startAddress);
    Iter end = pm->targetRegionIterEnd(startAddress+size);
    if(it != end)
    {
        os << "\tPointer to this chunk: ";
        for(; it != end; ++it)
        {
            VariablesType * vt =  it->second->getVariable();
            if(vt)
                os << "\t" << vt->getName() << " ";
            else
                os << "\t" << "Addr:0x" << hex << it->second->getSourceAddress() << " ";
        }
        os << endl;
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


    PointerManager * pm = rs->getPointerManager();

    addr_type from = m->getAddress();
    addr_type to = from + m->getSize();
    pm->deletePointerInRegion(from,to);
    pm->invalidatePointerToRegion(from,to);

    // successful free, erase allocation info from map
    mem.erase(m);
    delete m;
}



void MemoryManager::checkAccess(addr_type addr, size_t size, RsType * t, MemoryType * & mt, RuntimeViolation::Type vioType)
{
    RuntimeSystem * rs = RuntimeSystem::instance();


    mt = findContainingMem(addr,size);
    if(!mt)
    {
        stringstream desc;
        desc    << "Trying to access non-allocated MemoryRegion "
                << "(Address " << "0x" << hex << addr
                <<" of size " << dec << size << ")" << endl;

        MemoryType * possMatch = findPossibleMemMatch(addr);
        if(possMatch)
            desc << "Did you try to access this region:" << endl << *possMatch << endl;

        rs->violationHandler(vioType,desc.str());
    }

    if(t)
        mt->accessMemWithType(addr - mt->getAddress(),t);

}




void MemoryManager::checkRead(addr_type addr, size_t size, RsType * t)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    MemoryType * mt = NULL;
    checkAccess(addr,size,t,mt,RuntimeViolation::INVALID_READ);
    assert(mt);

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



void MemoryManager::checkWrite(addr_type addr, size_t size, RsType * t)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    MemoryType * mt = NULL;
    checkAccess(addr,size,t,mt,RuntimeViolation::INVALID_WRITE);
    assert(mt);

    assert(addr >= mt->getAddress());
    int from = addr - mt->getAddress();
    mt->initialize(from,from + size);
}


void MemoryManager::checkIfSameChunk(addr_type addr1, addr_type addr2, RsType * type)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    size_t typeSize = type->getByteSize();

    MemoryType * mem1 = NULL;
    MemoryType * mem2 = NULL;

    checkAccess(addr1,typeSize,type,mem1,RuntimeViolation::INVALID_READ);
    checkAccess(addr2,typeSize,type,mem2,RuntimeViolation::INVALID_READ);
    assert(mem1 && mem2);

    if(mem1 != mem2)
    {
        stringstream ss;
        ss << "Pointer changed allocation block from "
           << "0x" << hex << addr1 << " to "
           << "0x" << hex << addr2 << endl;

        rs->violationHandler(RuntimeViolation::POINTER_CHANGED_MEMAREA,ss.str());
        return;
    }

    MemoryType * mem = mem1;
    int off1 = addr1 - mem->getAddress();
    int off2 = addr2 - mem->getAddress();

    if(mem->isArrayPossible(off1,off2,type) )
        return;

    string chunk1 = mem1->getTypeAt(off1,typeSize);
    string chunk2 = mem2->getTypeAt(off2,typeSize);

    if(chunk1 == chunk2 && mem1 ==mem2)
        return; //pointer just changed offset in an array


    stringstream ss;
    ss << "A pointer changed the memory area (array or variable) which it points to (may be an error)" << endl << endl;

    ss << "Region1:  " <<  chunk1 << " at offset " << off1 << " in this Mem-Region:" <<  endl
                          << *mem1 << endl;

    ss << "Region2: " << chunk2 << " at offset " << off2 << " in this Mem-Region:" <<  endl
                          << *mem2 << endl;

    rs->violationHandler(RuntimeViolation::POINTER_CHANGED_MEMAREA,ss.str());
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

