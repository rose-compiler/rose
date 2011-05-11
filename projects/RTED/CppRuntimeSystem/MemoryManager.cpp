#include <sstream>
#include <iostream>
#include <memory>
#include <typeinfo>

#include "MemoryManager.h"

#include "CppRuntimeSystem.h"


static inline
TypeTracker* createTracker(bool distr, size_t size)
{
#ifdef WITH_UPC
  if (distr) return new DistributedStorage(size);
#endif /* WITH_UPC */

  return new ThreadStorage(size);
}

static inline
const void* toString(MemoryType::LocalPtr ptr)
{
  return ptr;
}


// -----------------------    MemoryType  --------------------------------------

MemoryType::MemoryType(Location _addr, size_t _size, AllocKind ak, bool distr, const SourcePosition& _pos)
: startAddress(_addr), origin(ak), allocPos(_pos),
  tracker( createTracker(distr, _size) )
{
  RuntimeSystem *   rs = RuntimeSystem::instance();

  if ( diagnostics::message(diagnostics::memory) )
  {
    std::stringstream msg;

    msg << "  ***** Allocate Memory :: [" << _addr
        << ", " << endAddress() << ")"
        << "  size = " << _size
        << "  @ (" << _pos.getLineInOrigFile() << ", " << _pos.getLineInTransformedFile() << ")";
    rs->printMessage(msg.str());
  }
}

void MemoryType::resize( size_t new_size )
{
    tracker->resize( new_size );
}

MemoryType::Location MemoryType::endAddress() const
{
  return ::add(startAddress, getSize(), tracker->distributed());
}

bool MemoryType::containsAddress(Location queryAddress) const
{
  //~ std::cerr << "$$$ contains? " << startAddress << " <= "
            //~ << queryAddress << " < " << endAddress() << "(" << getSize() << ")" << std::endl;

    return (  startAddress <= queryAddress
           && queryAddress < endAddress()
           );
}

MemoryType::Location MemoryType::lastValidAddress() const
{
  return ::add(startAddress, getSize()-1, tracker->distributed());
}

bool MemoryType::containsMemArea(Location queryAddr, size_t querySize) const
{
    return (  containsAddress(queryAddr)
           && containsAddress(::add(queryAddr, querySize - 1, tracker->distributed()))
           );
}

bool MemoryType::overlapsMemArea(Location queryAddr, size_t querySize) const
{
    return (  containsAddress(queryAddr)
           || containsAddress(::add(queryAddr, querySize - 1, tracker->distributed()))
           );
}

bool MemoryType::operator< (const MemoryType& other) const
{
    return (startAddress < other.startAddress);
}


bool MemoryType::isInitialized(Location addr, size_t len) const
{
    TypeTracker::InitData& init = tracker->initlistof(addr);
    const size_t           offset = addr.local - startAddress.local;
    const size_t           limit = offset + len;

    for (size_t i = offset; i < limit; ++i)
    {
        if (! init[i]) return false;
    }

    return true;
}

void MemoryType::initialize(Location addr, size_t len)
{
    TypeTracker::InitData& init = tracker->initlistof(addr);
    const size_t           offset = addr.local - startAddress.local;
    const size_t           limit = offset + len;

    for (size_t i = offset; i < limit; ++i)
    {
        init[i] = true;
    }
}

static inline
void insert( TypeTracker::TypeData& tmap, const TypeTracker::TypeData::value_type& val )
{
  tmap.insert(val);
}

void MemoryType::registerMemType(Location addr, RsType * type)
{
    RuntimeSystem *   rs = RuntimeSystem::instance();
    const bool        types_merged = checkAndMergeMemType(addr, type);

    if ( diagnostics::message(diagnostics::memory) )
    {
      std::stringstream msg;
      msg << "   ++ registerMemType at: " << addr << "  type: " << type->getName() << '\n'
          << "      ++ types_merged: " << types_merged;

      rs->printMessage(msg.str());
    }

    if ( !types_merged )
    {
        // type wasn't merged, some previously unknown portion of this
        // MemoryType has been registered with type
        assert(type);

        const bool distributed = tracker->distributed() && (dynamic_cast<RsArrayType*>(type) != NULL);

        insert( tracker->typesof(addr), TypeTracker::TypeData::value_type(addr.local, type) );

        // since we have knowledge about the type in memory, we also need to update the
        // information about "dereferentiable" memory regions i.e. pointer
        rs->getPointerManager()->createPointer(addr, type, distributed);
    }

    if ( diagnostics::message(diagnostics::memory) )
    {
      rs->printMessage("   ++ registerMemType done.");
    }
}

void MemoryType::forceRegisterMemType(Location addr, RsType* type )
{
    TypeData&  tmap = tracker->typesof(addr);
    TiIterPair type_range = getOverlappingTypeInfos(tmap, addr.local, type->getByteSize());

    tmap.erase(type_range.first, type_range.second);
    insert(tmap, TypeData::value_type(addr.local, type));
}

static
std::string composeVioMessage(MemoryType::LocalPtr newTiStart, size_t len, RsType* t, MemoryType& mt)
{
    std::stringstream msg;

    msg << "Tried to access the same memory region with different types" << std::endl;
    msg << "When trying to access (" << toString(newTiStart) << ","
        << toString(newTiStart + len) << ") "
        << "as type " << t->getName() << std::endl;
    msg << "Memory Region Info: " << std::endl;
    msg << mt << std::endl;

    return msg.str();
}

bool MemoryType::checkAndMergeMemType(Location addr, RsType * type)
{
    TypeData& tmap = tracker->typesof(addr);

    if (tmap.size() == 0) // no types registered yet
        return false;

    RuntimeSystem *   rs = RuntimeSystem::instance();
    LocalPtr          newTiStart = addr.local;
    size_t            newLength = type->getByteSize();

    if ( diagnostics::message(diagnostics::memory) )
    {
      std::stringstream msg;

      msg << "   ++ checkAndMergeMemType newTiStart: " << toString(newTiStart)
          << "      length: " << newLength << std::endl;

      rs->printMessage( msg.str() );
    }

    // Get a range of entries which overlap with [newTiStart,newTiEnd)
    TiIterPair res = getOverlappingTypeInfos(tmap, newTiStart, newLength);
    TiIter     itLower = res.first;
    TiIter     itUpper = res.second;

    // Case 0: There are types registered, but not ϵ [newTiStart, newTiEnd)
    if (itLower == itUpper)
      return false;

    //  Case 1: New entry is overlapped by one old entry,
    //  i.e. [itLower,itUpper) contains only one entry
    TiIter     incrementedLower = itLower;
    ++incrementedLower;

    if (incrementedLower == itUpper)
    {
        RsType*        oldType = itLower->second;
        LocalPtr       oldTiStart = itLower->first;
        size_t         oldLength = oldType->getByteSize();
        std::ptrdiff_t diff = oldTiStart - newTiStart;

        // was: if( (oldTiStart <= newTiStart) && (oldTiEnd >= newTiEnd) )
        if ((diff <= 0) && (newLength <= (diff + oldLength)))
        {
            //Check if new entry is consistent with old one
            bool new_type_ok = oldType->checkSubtypeRecursive( newTiStart - oldTiStart, type );

            if( !new_type_ok )
            {
                RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS, composeVioMessage(newTiStart, newLength, type, *this) );

                vio.descStream() << "Previously registered Type completely overlaps new Type in an inconsistent way:"
                                 << std::endl
                                 << "Containing Type " << oldType->getName()
                                 << " (" << toString(oldTiStart) << ","
                                 << toString(oldTiStart + oldLength) << ")" << std::endl;

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
    for(TypeData::iterator i = itLower; i != itUpper; ++i )
    {
        RsType*        curType = i->second;
        LocalPtr       curStart = i->first;
        size_t         curLength = curType->getByteSize();
        std::ptrdiff_t diff = curStart - newTiStart;

        // std::cout << "  >> " << diff << " #" << newLength << "  #" << curLength << std::endl;

        // was: if ((curStart < newTiStart) || (newTiEnd < curEnd))
        if ((diff < 0) || (newLength < (diff + curLength)))
        {
            RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS, composeVioMessage(newTiStart, newLength, type, *this) );

            vio.descStream() << "Previously registered Type overlaps new Type in an inconsistent way:"
                             << std::endl
                             << "Overlapping Type " << curType->getName()
                             << " (" << toString(curStart) << "," << toString(curStart + curLength) << ")"
                             << std::endl;
            rs->violationHandler(vio);
            return false;
        }

        RsType * sub =  type->getSubtypeRecursive(diff /*curStart - newTiStart*/, curLength);

        if (sub != curType)
        {
            RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS, composeVioMessage(newTiStart, newLength, type, *this) );

            vio.descStream() << "Newly registered Type completely overlaps a previous registered Type in an inconsistent way:"
                             << std::endl
                             << "Overlapping Type " << curType->getName()
                             << " (" << toString(curStart) << "," << toString(curStart + curLength) << ")"
                             << std::endl;

            rs->violationHandler(vio);
            return false;
        }
    }
    //All consistent - old typeInfos are replaced by big new typeInfo (merging)
    tmap.erase(itLower, itUpper);
    insert( tmap, TypeData::value_type(addr.local, type) );

    // if we have knowledge about the type in memory, we also need to update the
    // information about "dereferentiable" memory regions i.e. pointer
    const bool distributed = tracker->distributed() && (dynamic_cast<RsArrayType*>(type) != NULL);

    rs->getPointerManager()->createPointer(addr, type, distributed);

    return true;
}

static inline
bool isDistMem(const TypeTracker& tracker, const RsType& t)
{
  // std::cout << "  #@@ " << typeid(t).name() << std::endl;
  return tracker.distributed() && (dynamic_cast< const RsArrayType* >( &t ) != NULL);
}

static
MemoryType::Location typed_add(MemoryType::Location base, size_t offset, const TypeTracker& tracker, const RsType& t)
{
  return ::add(base, offset, isDistMem(tracker, t));
}

static inline
MemoryType::Location makeLocation(rted_thread_id thid, const char* addr)
{
  MemoryType::Location loc;

#ifdef WITH_UPC
  loc.thread_id = thid;
#endif /* WITH_UPC */
  loc.local = addr;

  return loc;
}


RsType* MemoryType::getTypeAt(Location addr, size_t size)
{
    TypeData&    tmap = tracker->typesof(addr);
    TiIterPair   type_range = getOverlappingTypeInfos(tmap, addr.local, size);
    TiIter       first_addr_type = type_range.first;
    TiIter       end = type_range.second;

    //~ std::cerr << "   distance = " << std::distance(first_addr_type, end) << std::endl;
    //~ std::cerr << "   addr = " << static_cast<const void*>(addr.local)
              //~ << "  first = " << static_cast<const void*>(first_addr_type->first) << std::endl;

    if( first_addr_type == end )
    {
        // We know nothing about the types at offset..+size
        return &RsType::UnknownType;
    }

    RsType*      res = NULL;
    TiIter       second_addr_type = first_addr_type;

    ++second_addr_type;
    // \pp   why can there be a number of overlapping typeinfos?
    //       possibly when continuing on an erroneous state?
    // \todo assert(second_addr_type == end)

    if ( second_addr_type == end && addr.local >= first_addr_type->first )
    {
      // We know only something about one type in this range
      RsType*    first_type = first_addr_type->second;
      assert(first_type);

      Location loc = makeLocation( tracker->baseThread(addr), first_addr_type->first );
      size_t   ofs = ::ofs( addr, loc, tracker->distributed() );

      // \pp I do not think that the third argument is very helpful
      //     In particular with multi-dimensional arrays, the returned
      //     array would not always be the top level
      res = first_type->getSubtypeRecursive( ofs, size, true /* stop-on-array */ );

      std::cerr << "   res = " << static_cast<const void*>(res)
                << "   " << typeid(first_type).name() << std::endl;
    }

    return res;
}

RsCompoundType* MemoryType::computeCompoundTypeAt(Location addr, size_t size)
{
    // \pp not sure why this is needed? should not we always have
    //     type information of a memory location + length?

    TypeData&       tmap = tracker->typesof(addr);
    TiIterPair      type_range = getOverlappingTypeInfos( tmap, addr.local, size );
    TiIter          first_addr_type = type_range.first;
    TiIter          end = type_range.second;

    // We don't know the complete type of the range, but we do know something.
    // Construct a compound type with the information we have.
    RsCompoundType* computed_type = new RsCompoundType( "Partially Known Type", size );

    for( TiIter i = first_addr_type; i != end; ++i)
    {
        //~ std::stringstream out;
        //~ out << rted_ThisThread() << "  first: " << static_cast<const void*>(i->first) << "   addr: " << static_cast<const void*>(addr.local) << " " << (i->first <= addr.local) << "\n";
        //~ std::cerr << out.str() << std::flush;

        // \bug \pp this seems a bug in the RTS implementation
        // addr and i can overlap in a way that addr can be either smaller
        //   or larger then i->first. Thus the following assert does not hold.
        // assert(i->first >= addr.local);
        if (i->first >= addr.local)
        {
          size_t  subtype_offset = i->first - addr.local;
          RsType* type = i -> second;

          computed_type->addMember( "", type, subtype_offset );
        }
    }

    return computed_type;
}


MemoryType::TiIter MemoryType::adjustPosOnOverlap(TiIter first, TiIter iter, LocalPtr from)
{
  if (iter == first) return iter;

  TiIter prev = iter;

  --prev;
  return prev->first + prev->second->getByteSize() <= from ? iter : prev;
}

MemoryType::TiIterPair MemoryType::getOverlappingTypeInfos(TypeData& tmap, LocalPtr from, size_t len)
{
    // This function assumes that the typeInfo ranges do not overlap

    // end is the iterator which points to typeInfo with address >= (from + len)
    TiIter first = tmap.begin();
    TiIter last = tmap.end();
    TiIter ub = tmap.lower_bound(from + len);

    // the same for the beginning, but there we need the previous map entry
    TiIter lb = tmap.lower_bound(from);

    lb = adjustPosOnOverlap(first, lb, from);

    return TiIterPair(lb, ub);
}



std::string MemoryType::getInitString() const
{
    const char* res = NULL;

    switch (tracker->initializationStatus())
    {
      case TypeTracker::none: res = "Not initialized  "; break;
      case TypeTracker::some: res = "Part. initialized"; break;
      case TypeTracker::all:  res = "Initialized      "; break;
      default: assert(false);
    }

    assert(res != NULL);
    return res;
}

void MemoryType::print() const
{
    print( std::cerr );
}

// extra print function because operator<< cannot be member-> no access to privates
void MemoryType::print(std::ostream & os) const
{
    typedef PointerManager::TargetToPointerMap::const_iterator Iter;

    os << startAddress
       << " Size " << std::dec << getSize()
       <<  "\t" << getInitString() << "\tAllocated here: " << allocPos
       << std::endl;

    PointerManager * pm = RuntimeSystem::instance()->getPointerManager();
    Iter             it =  pm->targetRegionIterBegin(startAddress);
    Iter             end = pm->targetRegionIterEnd(add(startAddress, getSize(), tracker->distributed()));

    if(it != end)
    {
        os << "\tPointer to this chunk: ";
        for(; it != end; ++it)
        {
            VariablesType * vt =  it->second->getVariable();
            if(vt)
                os << "\t" << vt->getName() << " ";
            else
                os << "\t" << "Addr:0x" << it->second->getSourceAddress() << " ";
        }
        os <<std::endl;
    }

/*
 *  \pp \todo \fix
    if (typeInfo.size() > 0)
    {
        os << "\t" << "Type Info" <<std::endl;
        for(TypeData::const_iterator i = typeInfo.begin(); i != typeInfo.end(); ++i)
            os << "\t" << i->first << "\t" << i->second->getName() <<std::endl;
    }
    */
}

std::ostream& operator<< (std::ostream &os, const MemoryType & m)
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
}


MemoryType* MemoryManager::findPossibleMemMatch(Location addr)
{
  if (mem.size()==0)
    return NULL;

  // search the memory chunk, we get the chunk which has equal or greater startAdress
  MemoryTypeSet::iterator it = mem.lower_bound(addr);

  // case 0: we are at the end and map.size>0, so return last element
  if (it == mem.end())
  {
    --it;
    return &(it->second);
  }

  //case 1: addr is startAdress of a chunk, exact match
  if (it->first == addr)
  {
    return &(it->second);
  }

  //case 2: addr is greater than a chunk startadress
  //        try to get next smaller one
  assert(addr < it->first);

  if (it == mem.begin())
    return NULL;

  --it;
  return &(it->second);
}

MemoryType* MemoryManager::findContainingMem(Location addr, size_t size)
{
    MemoryType * res = findPossibleMemMatch(addr);

    return (res && res->containsMemArea(addr,size)) ?  res  : NULL;
}

bool MemoryManager::existOverlappingMem(Location addr, size_t size)
{
    MemoryType * res = findPossibleMemMatch(addr);

    return res && res->overlapsMemArea(addr, size);
}



MemoryType* MemoryManager::allocateMemory(Location adrObj, size_t szObj, MemoryType::AllocKind kind, bool distr, const SourcePosition& pos)
{
    if (szObj == 0)
    {
        RuntimeSystem::instance()->violationHandler(RuntimeViolation::EMPTY_ALLOCATION,"Tried to call malloc/new with size 0\n");
        return NULL;
    }

    if (existOverlappingMem(adrObj, szObj))
    {
        // the start address of new chunk lies in already allocated area
        RuntimeSystem::instance()->violationHandler(RuntimeViolation::DOUBLE_ALLOCATION);
        return NULL;
    }

    MemoryType                tmp(adrObj, szObj, kind, distr, pos);
    MemoryTypeSet::value_type v(adrObj, tmp);

    return &mem.insert(v).first->second;
}

static
std::string allocDisplayName(AllocKind ak)
{
  const char* res = NULL;

  switch (ak)
  {
    case akStack          : res = "Stack"; break;
    case akCHeap          : res = "malloc/calloc"; break;

    /* C++ */
    case akCxxNew         : res = "operator new"; break;
    case akCxxArrayNew    : res = "operator new[]"; break;

    /* UPC */
    case akUpcAlloc       : res = "upc_alloc/upc_local_alloc"; break;
    case akUpcGlobalAlloc : res = "upc_global_alloc"; break;
    case akUpcAllAlloc    : res = "upc_all_alloc"; break;
    default               : res = "unknown allocation method";
  }

  assert(res != NULL);
  return res;
}

static
std::string freeDisplayName(AllocKind ak)
{
  const char* res = NULL;

  switch (ak)
  {
    case akStack          : res = "stack (rted bug?) free"; break;
    case akCHeap          : res = "free"; break;

    /* C++ */
    case akCxxNew         : res = "operator delete"; break;
    case akCxxArrayNew    : res = "operator delete[]"; break;

    /* UPC */
    case akUpcSharedHeap  : res = "upc_free"; break;
    default               : res = "unknown deallocation method";
  }

  assert(res != NULL);
  return res;
}

static
std::string
alloc_free_mismatch(MemoryType& m, AllocKind howAlloced, AllocKind howFreed)
{
  std::stringstream desc;

  if (howAlloced == akStack)
  {
    desc << "Stack memory was explicitly freed (0x" << m << ")" << std::endl;
  }
  else
  {
    assert((howAlloced > akStack) && (howFreed > akStack));

    desc << "Memory allocated with '" << allocDisplayName(howAlloced) << "' "
         << "but deallocated with '"  << freeDisplayName(howFreed)    << "' "
         << "0x" << m
         << std::endl;
  }

  return desc.str();
}


void MemoryManager::freeMemory(Location addr, MemoryType::AllocKind freekind)
{
    RuntimeSystem* rs = RuntimeSystem::instance();
    MemoryType*    m = findContainingMem(addr, 1);

    // free on unknown address
    if (m == NULL)
    {
        std::stringstream desc;
        desc << freeDisplayName(freekind);
        desc << " was called with unknown address " << addr <<std::endl;
        desc << " Allocated Memory Regions:" << std::endl;
        print(desc);
        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    }

    // free inside an allocated memory block
    if (m->beginAddress() != addr)
    {
        std::stringstream desc;
        desc << freeDisplayName(freekind);
        desc << " was called with an address inside of an allocated block (Address:" << addr <<")" <<std::endl;
        desc << "Allocated Block: " << *m <<std::endl;

        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    }

    // free stack memory explicitly (i.e. not via exitScope)
    MemoryType::AllocKind allockind = m->howCreated();

    // allockind can be different from freekind
    //   e.g., upc_free subsumes all upc allocations
    if ((allockind & freekind) != freekind)
    {
      std::string msg = alloc_free_mismatch(*m, allockind, freekind);
      rs->violationHandler(RuntimeViolation::INVALID_FREE, msg);
      return;
    }

    PointerManager* pm = rs->getPointerManager();

    pm->deletePointerInRegion( *m );
    pm->invalidatePointerToRegion( *m );

    // successful free, erase allocation info from map
    mem.erase(m->beginAddress());
}



MemoryType* MemoryManager::checkAccess(Location addr, size_t size, RsType * t, RuntimeViolation::Type vioType)
{
    RuntimeSystem*    rs = RuntimeSystem::instance();
    MemoryType *const res = findContainingMem(addr,size);

    if(!res)
    {
        std::stringstream desc;
        desc << "Trying to access non-allocated MemoryRegion "
             << "(Address " << addr << " of size " << std::dec << size << ")"
             << std::endl;

        const MemoryType* possMatch = findPossibleMemMatch(addr);
        if (possMatch)
            desc << "Did you try to access this region:" << std::endl << *possMatch <<std::endl;

        rs->violationHandler(vioType, desc.str());
    }

    if (t)
    {
      if ( diagnostics::message(diagnostics::memory) )
      {
        std::stringstream msg;

        msg << "   ++ found memory addr : " << addr << " size:" << ToString(size);
        rs->printMessage(msg.str());
      }

      res->registerMemType(addr, t);
    }

    return res;
}




void MemoryManager::checkRead(Location addr, size_t size, RsType * t)
{
    // \pp \note t seems to be always NULL

    const MemoryType* mt = checkAccess(addr,size,t,RuntimeViolation::INVALID_READ);
    assert(mt != NULL && mt->beginAddress() <= addr);

    if(! mt->isInitialized(addr, size) )
    {
      RuntimeSystem * rs = RuntimeSystem::instance();

      std::stringstream desc;
      desc << "Trying to read from uninitialized MemoryRegion "
           << "(Address " << "0x" << addr
           << " of size " << std::dec << size << ")"
           << std::endl;

      rs->violationHandler(RuntimeViolation::INVALID_READ,desc.str());
    }
}



void MemoryManager::checkWrite(Location addr, size_t size, RsType * t)
{
    RuntimeSystem*    rs = RuntimeSystem::instance();

    if ( diagnostics::message(diagnostics::memory) )
    {
      std::stringstream msg;

      msg << "   ++ checkWrite : " << addr << " size:" << size;
      rs->printMessage(msg.str());
    }

    MemoryType *      mt = checkAccess(addr, size, t, RuntimeViolation::INVALID_WRITE);
    assert(mt && mt->beginAddress() <= addr);

    // \pp \todo this implementation does not consider UPC blocking
    //           consider modifing the initialize interface
    mt->initialize(addr, size);

    if ( diagnostics::message(diagnostics::memory) )
    {
      rs->printMessage("   ++ checkWrite done.");
    }
}

bool MemoryManager::isInitialized(Location addr, size_t size)
{
    const MemoryType* mt = checkAccess(addr,size,NULL,RuntimeViolation::INVALID_READ);

    return mt->isInitialized(addr, size);
}


bool MemoryManager::checkIfSameChunk(Location addr1, Location addr2, RsType * type)
{
  return checkIfSameChunk( addr1, addr2, type->getByteSize() );
}

bool
MemoryManager::checkIfSameChunk( Location addr1, Location addr2, size_t typeSize, RuntimeViolation::Type violation )
{
  RuntimeSystem *        rs = RuntimeSystem::instance();
  RuntimeViolation::Type access_violation = violation;

  if (access_violation != RuntimeViolation::NONE) access_violation = RuntimeViolation::INVALID_READ;

  MemoryType *           mem1 = checkAccess(addr1, typeSize, NULL, access_violation);

  // check that addr1 and addr2 point to the same base location
  {
    MemoryType *         mem2 = checkAccess(addr2, typeSize, NULL, access_violation);

    if (violation == RuntimeViolation::NONE && !(mem1 && mem2))
    {
      return false;
    }

    assert(mem1 && mem2);
    if (mem1 != mem2)
    {
        std::stringstream ss;
        ss << "Pointer changed allocation block from "
           << addr1 << " to " << addr2
           << std::endl;

        rs->violationHandler( violation, ss.str() );
        return false;
    }
  }

  // so far we know that addr1, addr2 have been allocated within the same chunk
  // now, test for same array chunks

  // \pp as far as I understand, getTypeAt returns the innermost type
  //     that overlaps a certain memory region [addr1, typeSize).
  //     The current implementation stops at the highest level of array...
  RsType*               type1 = mem1 -> getTypeAt( addr1, typeSize );
  RsType*               type2 = mem1 -> getTypeAt( addr2, typeSize );
  std::auto_ptr<RsType> guard1(NULL);
  std::auto_ptr<RsType> guard2(NULL);

  if( !type1 ) {
      //~ std::cerr << "type1! " << std::endl;
      type1 = mem1 -> computeCompoundTypeAt( addr1, typeSize );
      guard1.reset(type1);
  }

  if( !type2 ) {
      //~ std::cerr << "type2! " << std::endl;
      type2 = mem1 -> computeCompoundTypeAt( addr2, typeSize );
      guard2.reset(type2);
  }

  assert( type1 && type2 );

  bool         accessOK = type1->isConsistentWith( *type2 );
  RsArrayType* array = (accessOK ? dynamic_cast< RsArrayType* >( type1 ) : 0);

  //~ std::cerr << "accOK: " << accessOK << std::endl;

  if (array)
  {
    // \pp \todo not sure why bounds checking is based on a relative address (addr1)
    //     and not on absolute boundaries of the chunk where this address is located...
    //     e.g., addr2 < array(addr).lb || array(addr).ub < (addr2+typeSize)
    //     - a reason is that we want to bounds check within a multi-dimensional
    //       array; a sub-array might not start at the same address as the
    //       allocated chunk (which is always at [0][0]...)
    // \bug  this works fine for arrays, but it does not seem to be OK for
    //       pointers; in which case addr1 might point in the middle of a chunk.
    const Location arrlb = addr1;
    const Location arrub = ::add(addr1, array->getByteSize(), mem1->tracker->distributed());
    const Location elemlb = addr2;
    const Location elemub = typed_add(addr2, typeSize, *mem1->tracker, *type2);

    //~ std::cerr << " $2=" << addr2 << " < $1=" << addr1 << std::endl;
    //~ std::cerr << " $1 + " << array->getByteSize() << " = " << arrub
              //~ << " < $2 + " << typeSize << " = " << elemub << std::endl;

    // the array element might be before it [ -1 ]
    // ... or after the array [ N ]...
    if ( (elemlb < arrlb) || (arrub < elemub) )
    {
      // out of bounds error (e.g. int[2][3], ref [0][3])
      accessOK = false;
    }

    //~  std::cerr << " res = " << accessOK << std::endl;

    /* was:
    if  !(  addr1 + array -> getByteSize() >= addr2 + typeSize  // the array element might be after the array [ N ]...
         && addr1 <= addr2 // ... or before it [ -1 ]
         *   (12)     (8)
         )
    {
      // out of bounds error (e.g. int[2][3], ref [0][3])
      consistent = false;
    }
    */
  }

  if (!accessOK)
    failNotSameChunk( type1, type2, addr1, addr2, mem1, mem1, violation );

  return accessOK;
}

void MemoryManager::failNotSameChunk(
        RsType* type1,
        RsType* type2,
        Location addr1,
        Location addr2,
        MemoryType* mem1,
        MemoryType* mem2,
    RuntimeViolation::Type violation) {

    RuntimeSystem * rs = RuntimeSystem::instance();

    std::stringstream ss;
    ss << "A pointer changed the memory area (array or variable) which it points to (may be an error)"
       << std::endl << std::endl;

    ss << "Region1:  " <<  *type1 << " at " << addr1
       << " in this Mem-Region:" << *mem1
       << std::endl;

    ss << "Region2: " << *type2 << " at " << addr2
       << " in this Mem-Region:" << *mem2
       << std::endl;

    rs->violationHandler( violation ,ss.str() );
}






void MemoryManager::checkForNonFreedMem() const
{
    std::stringstream desc;
    if(mem.size() !=0)
    {
        desc << "Program terminated and the following allocations were not freed:" <<std::endl;
        print(desc);

        RuntimeSystem::instance()->violationHandler(RuntimeViolation::MEMORY_LEAK,desc.str());
    }
}


MemoryType * MemoryManager::getMemoryType(Location addr)
{
    MemoryType * possibleMatch = findPossibleMemMatch(addr);

    return(
        possibleMatch != NULL
        && possibleMatch->beginAddress() == addr
            ? possibleMatch
            : NULL
    );
}


void MemoryManager::print(std::ostream& os) const
{
    if (mem.size() ==0 )
    {
        os << "Memory Manager: No allocations" << std::endl;
        return;
    }

    os << std::endl;
    os << "------------------------------- Memory Manager Status -----------------------------" << std::endl << std::endl;
    for (MemoryTypeSet::const_iterator i = mem.begin(); i != mem.end(); ++i)
        os << i->second << std::endl;

    os << "-----------------------------------------------------------------------------------" << std::endl;
    os << std::endl;
}

std::ostream& operator<< (std::ostream &os, const MemoryManager& m)
{
    m.print(os);
    return os;
}


  static
  TypeTracker::InitStatus initializationStatus(const TypeTracker::InitData& initlist)
  {
    assert(initlist.size() > 0);

    bool   allset = true;
    bool   allunset = true;
    size_t last = initlist.size();
    size_t i = 0;

    while (i < last && (allset || allunset))
    {
      if (initlist[i]) allunset = false;
      else allset = false;

      ++i;
    }

    assert(!(allset && allunset));

    if (allset) return TypeTracker::all;
    if (allset) return TypeTracker::none;
    return TypeTracker::some;
  }

#ifdef WITH_UPC

  inline
  void DistributedStorage::assert_threadno(Location loc)
  {
    assert(loc.thread_id < rted_Threads());
  }

  DistributedStorage::TypeData& DistributedStorage::typesof(Location loc)
  {
    assert_threadno(loc);

    return types[loc.thread_id];
  }

  DistributedStorage::InitData& DistributedStorage::initlistof(Location loc)
  {
    assert_threadno(loc);

    return initialized[loc.thread_id];
  }

  void DistributedStorage::resize( size_t )
  {
    assert(false);
  }

  size_t DistributedStorage::baseThread(Location l) const
  {
    return l.thread_id;
  }

  DistributedStorage::InitStatus DistributedStorage::initializationStatus() const
  {
    assert( initialized.size() );

    InitDataContainer::const_iterator aa = initialized.begin();
    InitDataContainer::const_iterator zz = initialized.end();
    InitStatus                        is = ::initializationStatus(*aa);

    ++aa;
    while (is != some && aa != zz)
    {
      InitStatus is2 = ::initializationStatus(*aa);

      ++aa;
      if (is2 != is) is = TypeTracker::some;
    }

    return is;
  }

  DistributedStorage* DistributedStorage::clone() const
  {
    return new DistributedStorage(*this);
  }
#endif /* WITH_UPC */

  ThreadStorage::TypeData& ThreadStorage::typesof(Location loc)
  {
    return types;
  }

  ThreadStorage::InitData& ThreadStorage::initlistof(Location loc)
  {
    return initialized;
  }

  void ThreadStorage::resize( size_t newsize )
  {
    assert( newsize >= size );
    size = newsize;
    initialized.resize(size, false);
  }

  ThreadStorage::InitStatus ThreadStorage::initializationStatus() const
  {
    return ::initializationStatus(initialized);
  }

  ThreadStorage* ThreadStorage::clone() const
  {
    return new ThreadStorage(*this);
  }

  size_t ThreadStorage::baseThread(Location l) const
  {
#ifdef WITH_UPC
    return l.thread_id;
#else  /* WITH_UPC */
    return 0;
#endif /* WITH_UPC */
  }
