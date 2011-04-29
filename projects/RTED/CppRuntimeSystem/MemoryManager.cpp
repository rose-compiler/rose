#include "MemoryManager.h"

#include <boost/foreach.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>


#include "CppRuntimeSystem.h"

using namespace std;


// -----------------------    MemoryType  --------------------------------------
template<typename T>
std::string HexToString(T t){
  std::ostringstream myStream; //creates an ostringstream object
  myStream <<  std::hex <<  t ;
  return myStream.str(); //returns the string form of the stringstream object
}
template<typename T>
std::string ToString(T t){
std::ostringstream myStream; //creates an ostringstream object
myStream << t << std::flush;
return myStream.str(); //returns the string form of the stringstream object
}

MemoryType::MemoryType(addr_type _addr, size_t _size, const SourcePosition & _pos, bool _onStack, bool _fromMalloc)
    : startAddress(_addr), size(_size), allocPos(_pos), onStack(_onStack), fromMalloc( _fromMalloc )
{
    // not memory has been initialized by default
	RuntimeSystem * rs = RuntimeSystem::instance();
	    rs->printMessage("	***** Allocate Memory ::: "+HexToString(_addr)+
					"  size:"+ToString(_size)+"  pos1:"+ToString(_pos.getLineInOrigFile()));
    initialized.assign(size,false);
}

MemoryType::MemoryType(addr_type _addr, size_t _size, bool _onStack, bool _fromMalloc,
                       const std::string & file, int line1, int line2)
    : startAddress(_addr),
      size(_size),
      allocPos(file,line1,line2),
      onStack(_onStack),
      fromMalloc( _fromMalloc )
{
}

MemoryType::MemoryType(addr_type addr, bool _onStack)
    : startAddress(addr), size(0), allocPos("unknown",-1,-1), onStack(_onStack)
{
}

MemoryType::~MemoryType()
{

}

void MemoryType::resize( size_t new_size ) {
    assert( new_size >= size );
    size = new_size;
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
    assert(offsetFrom < (int)size && offsetTo <= (int)size);

    for(int i= offsetFrom; i<offsetTo; i++)
        if(! initialized[i])
            return false;

    return true;
}

void MemoryType::initialize(int offsetFrom, int offsetTo)
{
    assert(offsetFrom<offsetTo);
    assert(offsetFrom < (int)size && offsetTo <= (int)size);

    for(int i=offsetFrom; i<offsetTo; i++)
        initialized[i]=true;
}



void MemoryType::registerMemType(addr_type offset, RsType * type)
{
    RuntimeSystem * rs = RuntimeSystem::instance();
    rs->printMessage("   ++ registerMemType at offset: "+ToString(offset)+"  type: "+type->getName());
    bool types_merged = checkAndMergeMemType( offset, type );
    rs->printMessage("   ++ types_merged: "+types_merged);

    if( !types_merged ) {
        // type wasn't merged, some previously unknown portion of this
        // MemoryType has been registered with type

        typeInfo.insert(make_pair<int,RsType*>(offset,type));
        // if we have knowledge about the type in memory, we also need to update the
        // information about "dereferentiable" memory regions i.e. pointer
        RuntimeSystem::instance()->getPointerManager()->createPointer(startAddress+offset,type);
    }
    rs->printMessage("   ++ registerMemType done.");
}

void MemoryType::forceRegisterMemType( addr_type offset, RsType* type ) {
    TiIterPair type_range = getOverlappingTypeInfos(
        offset, offset + type -> getByteSize() );

    TiIter incrementedLower = type_range.first;
    ++incrementedLower;

    typeInfo.erase( type_range.first, type_range.second );
    typeInfo.insert( make_pair< int, RsType* >( offset, type ));
}

bool MemoryType::checkAndMergeMemType(addr_type offset, RsType * type)
{
    RuntimeSystem * rs = RuntimeSystem::instance();
    if(typeInfo.size() ==0) // no types registered yet
        return false;

    int newTiStart = offset;
    int newTiEnd = offset + type->getByteSize();
    rs->printMessage("   ++ checkAndMergeMemType newTiStart: "+ToString(newTiStart)+
				"  newTiEnd: "+ToString(newTiEnd));

    stringstream ss;
    ss << "Tried to access the same memory region with different types" << endl;
    ss << "When trying to access (" << newTiStart << "," << newTiEnd << ") "
       << "as type " << type->getName() << endl
       << "Memory Region Info: " << endl << *this << endl;

    RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS,ss.str());

    // Get a range of entries which overlap with [newTiStart,newTiEnd)
    TiIterPair res = getOverlappingTypeInfos(newTiStart,newTiEnd);
    TiIter itLower = res.first;
    TiIter itUpper = res.second;

    // Case 0: There are types registered, but not ϵ [newTiStart, newTiEnd)
    if( itLower == typeInfo.end() )
        return false;

    //  Case 1: New entry is overlapped by one old entry,
    //  i.e. [itLower,itUpper) contains only one entry
    TiIter incrementedLower = itLower;
    ++incrementedLower;

    if(incrementedLower == itUpper)
    {
        rs->printMessage("     ++ incrementedLower == itUpper ");
        int oldTiStart = itLower->first;
        int oldTiEnd = oldTiStart + itLower->second->getByteSize();
        RsType * oldType = itLower->second;
        if( oldTiStart <= newTiStart && oldTiEnd >= newTiEnd )
        {

            rs->printMessage("    oldTiStart <= newTiStart && oldTiEnd >= newTiEnd ");
            //Check if new entry is consistent with old one
            bool new_type_ok = oldType -> checkSubtypeRecursive( newTiStart - oldTiStart, type );
            rs->printMessage("      new_type_ok : "+ToString(new_type_ok));
            if( !new_type_ok )
            {
                vio.descStream() << "Previously registered Type completely overlaps new Type in an inconsistent way:" << endl
                                 << "Containing Type " << oldType->getName()
                                 << " (" << oldTiStart << "," << oldTiEnd << ")" << endl;

                rs->violationHandler(vio);
                return false;
            }
            else
            {
                // successful, no need to add because the type is already contained in bigger one
                // effectively type has already been “merged”.
                return true;
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
            return false;
        }

        RsType * sub =  type->getSubtypeRecursive(curStart - newTiStart,curType->getByteSize());
        if (sub != curType)
        {
            vio.descStream() << "Newly registered Type completely overlaps a previous registered Type in an inconsistent way:" << endl
                             << "Overlapping Type " << curType->getName()
                             << " (" << curStart << "," << curEnd << ")" << endl;

            rs->violationHandler(vio);
            return false;
        }
    }
    //All consistent - old typeInfos are replaced by big new typeInfo (merging)
    typeInfo.erase(itLower,itUpper);

    typeInfo.insert(make_pair<int,RsType*>(offset,type));


    // if we have knowledge about the type in memory, we also need to update the
    // information about "dereferentiable" memory regions i.e. pointer
    RuntimeSystem::instance()->getPointerManager()->createPointer(startAddress+offset,type);
    return true;
}


RsType* MemoryType::getTypeAt(addr_type offset, size_t size)
{
    TiIterPair type_infos = getOverlappingTypeInfos( offset, offset + size );
    TiIter first_addr_type = type_infos.first;
    TiIter end = type_infos.second;

    if( first_addr_type == typeInfo.end() )
        // We know nothing about the types at offset..+size
        return &RsType::UnknownType;

    TiIter second_addr_type = first_addr_type;
    ++second_addr_type;

    if( second_addr_type == end ) {
        // We know only something about one type in this range
        RsType* first_type = first_addr_type -> second;
        RsType* maybe_return = 
            first_type 
                -> getSubtypeRecursive(
                        offset - first_addr_type -> first,
                        size,
                        true );
       if( maybe_return )
           // We know about a type that fills the entire range offset..size
           // This might be a member of a class.
           return maybe_return;
    }
    return NULL;
}

void MemoryType::computeCompoundTypeAt( addr_type offset, size_t size, RsType* &rv ) {
    TiIterPair type_infos = getOverlappingTypeInfos( offset, offset + size );
    TiIter first_addr_type = type_infos.first;
    TiIter end = type_infos.second;

    // We don't know the complete type of the range, but we do know something.
    // Construct a compound type with the information we have.
    RsCompoundType &computed_type = 
        *( new RsCompoundType( "Partially Known Type", size ));
    for( TiIter i = first_addr_type; i != end; ++i) {
        addr_type subtype_offset = i -> first - offset;
        RsType* type = i -> second;

        computed_type.addMember( "", type, subtype_offset );
    }
    rv = &computed_type;
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
    if(it != end && it->first + it->second->getByteSize() < from + 1)
        ++it;

    //for(TiIter i = it; i != end; ++i)
    //    cerr  << "Overlap: \t "<< i->first  << "\t" << i->second->getName() << endl ;

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

void MemoryType::print() const
{
    print( cerr );
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
    for(MemoryTypeSet::iterator i = mem.begin(); i != mem.end(); ++i)
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


void MemoryManager::freeMemory(addr_type addr, bool onStack, bool fromMalloc)
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

    // memory was malloc-d, but freed via delete
    if( m -> wasFromMalloc() && !fromMalloc ) {
        stringstream desc;
        desc << "Memory allocated via malloc freed with delete or delete[] (0x"
             << *m << endl;

        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    // memory was created via new, but freed via 'free' function
    } else if( !(m -> wasFromMalloc()) && fromMalloc ) {
        stringstream desc;
        desc << "Memory allocated via new freed with 'free' function (0x" 
             << *m << endl;

        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    }


    PointerManager * pm = rs->getPointerManager();

    addr_type from = m->getAddress();
    addr_type to = from + m->getSize();
    pm->deletePointerInRegion( from, to, true );
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

    if(t) {
        rs->printMessage("   ++ found memory addr : "+HexToString(addr)+" size:"+ToString(size));
    	mt->registerMemType(addr - mt->getAddress(),t);
    }
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
    rs->printMessage("   ++ checkWrite : "+HexToString(addr)+" size:"+ToString(size));
    MemoryType * mt = NULL;
    checkAccess(addr,size,t,mt,RuntimeViolation::INVALID_WRITE);
    assert(mt);

    assert(addr >= mt->getAddress());
    int from = addr - mt->getAddress();
    mt->initialize(from,from + size);
    rs->printMessage("   ++ checkWrite done.");
}

bool MemoryManager::isInitialized(addr_type addr, size_t size)
{
    MemoryType * mt = NULL;
    checkAccess(addr,size,NULL,mt,RuntimeViolation::INVALID_READ);

    int offset = addr - mt->getAddress();
    return mt->isInitialized(offset,offset+size);
}


bool MemoryManager::checkIfSameChunk(addr_type addr1, addr_type addr2, RsType * type)
{
	return checkIfSameChunk( addr1, addr2, type->getByteSize() );
}

bool MemoryManager::checkIfSameChunk(
		addr_type addr1,
		addr_type addr2,
		size_t typeSize,
		RuntimeViolation::Type violation) {

    RuntimeSystem * rs = RuntimeSystem::instance();

    MemoryType * mem1 = NULL;
    MemoryType * mem2 = NULL;

    RuntimeViolation::Type access_violation
        = ( violation == RuntimeViolation::NONE )
            ? RuntimeViolation::NONE
            : RuntimeViolation::INVALID_READ;
    checkAccess(addr1,typeSize,NULL,mem1, access_violation );
    checkAccess(addr2,typeSize,NULL,mem2, access_violation );

    if(     violation == RuntimeViolation::NONE
            && !( mem1 && mem2 )) {
        return false;
    }

    assert(mem1 && mem2);

    if(mem1 != mem2)
    {
        stringstream ss;
        ss << "Pointer changed allocation block from "
           << "0x" << hex << addr1 << " to "
           << "0x" << hex << addr2 << endl;

        rs->violationHandler( violation, ss.str() );
        return false;
    }

    MemoryType * mem = mem1;
    int off1 = addr1 - mem->getAddress();
    int off2 = addr2 - mem->getAddress();

    RsType* type1 = mem1 -> getTypeAt( off1, typeSize );
    RsType* type2 = mem2 -> getTypeAt( off2, typeSize );

    vector< RsType* > to_delete;
    if( !type1 ) {
        mem1 -> computeCompoundTypeAt( off1, typeSize, type1 );
        to_delete.push_back( type1 );
    } if( !type2 ) {
        mem1 -> computeCompoundTypeAt( off2, typeSize, type2 );
        to_delete.push_back( type2 );
    }

    assert( type1 );
    assert( type2 );

    bool failed = false;
    if( type1 -> isConsistentWith( *type2 )) {
       
        RsArrayType* array = dynamic_cast< RsArrayType* >( type1 );
        if(		array 
				&& !(
					// the array element might be after the array [ N ]...
					off1 + array -> getByteSize() >= off2 + typeSize
					// ... or before it [ -1 ]
					&& off2 >= off1 ))
            // out of bounds error (e.g. int[2][3], ref [0][3])
            failed = true;
    } else
        failed = true;

    if( failed )
        failNotSameChunk( type1, type2, off1, off2, mem1, mem2, violation );

    BOOST_FOREACH( RsType* target, to_delete ) {
        delete target;
    }

	return !failed;
}

void MemoryManager::failNotSameChunk(
        RsType* type1, 
        RsType* type2,
        addr_type off1,
        addr_type off2,
        MemoryType* mem1,
        MemoryType* mem2,
		RuntimeViolation::Type violation) {

    RuntimeSystem * rs = RuntimeSystem::instance();

    stringstream ss;
    ss << "A pointer changed the memory area (array or variable) which it points to (may be an error)" << endl << endl;

    ss << "Region1:  " <<  *type1 << " at offset " << off1 << " in this Mem-Region:" <<  endl
                          << *mem1 << endl;

    ss << "Region2: " << *type2 << " at offset " << off2 << " in this Mem-Region:" <<  endl
                          << *mem2 << endl;

    rs->violationHandler( violation ,ss.str() );
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

    return(
        possibleMatch != NULL
        && possibleMatch->getAddress() == addr
            ? possibleMatch 
            : NULL
    );
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
    for(MemoryTypeSet::iterator i = mem.begin(); i != mem.end(); ++i)
        os << **i << endl;

    os << "-----------------------------------------------------------------------------------" << endl;
    os << endl;
}

ostream& operator<< (ostream &os, const MemoryManager & m)
{
    m.print(os);
    return os;
}

