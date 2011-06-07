#include <sstream>
#include <iostream>
#include <memory>
#include <typeinfo>

#include "MemoryManager.h"

#include "CppRuntimeSystem.h"
#include "rtedsync.h"


static inline
const void* toString(MemoryType::LocalPtr ptr)
{
  return ptr;
}

template <class Container>
struct ContainerTraits
{
  typedef typename Container::iterator    Iterator;
};

template <class Container>
struct ContainerTraits<const Container>
{
  typedef typename Container::const_iterator    Iterator;
};

template <class X, class Y>
struct ConstLike
{
  typedef Y type;
};

template <class X, class Y>
struct ConstLike<const X, Y>
{
  typedef const Y type;
};

static
std::ptrdiff_t byte_offset(Address base, Address elem, size_t blocksize, size_t blockofs);

/// \brief if the entry at (iter-1) overlaps with from, use (iter-1)
///        as the start of the range.
template <class Iterator>
static
Iterator adjustPosOnOverlap(Iterator first, Iterator iter, size_t from)
{
  if (iter == first) return iter;

  Iterator prev = iter;
  --prev;

  return prev->first + prev->second->getByteSize() <= from ? iter : prev;
}


/// \brief  Determines all typeinfos which intersect the defined offset-range [from,to)
///         "to" is exclusive i.e. typeInfos with startOffset==to are not included
/// \return depending on the constness of tmap, either a pair of iterators or
///         or const_iterators
template <class TypeMap>
static
std::pair<typename ContainerTraits<TypeMap>::Iterator, typename ContainerTraits<TypeMap>::Iterator>
getOverlappingTypeInfos(TypeMap& tmap, size_t from, size_t len)
{
    typedef typename ContainerTraits<TypeMap>::Iterator Iterator;

    Iterator first = tmap.begin();
    Iterator last = tmap.end();
    Iterator ub = tmap.lower_bound(from + len);

    // the same for the beginning, but there we need the previous map entry
    Iterator lb = tmap.lower_bound(from);

    lb = adjustPosOnOverlap(first, lb, from);

    return std::make_pair(lb, ub);
}

// -----------------------    MemoryType  --------------------------------------

MemoryType::MemoryType(Location _addr, size_t _size, AllocKind ak, long blsz, const SourcePosition& _pos)
: startAddress(_addr), origin(ak), allocPos(_pos), typedata(), initdata(_size), blocksize(blsz)
{
  assert(_size && getSize());

  RuntimeSystem*   rs = RuntimeSystem::instance();

  if ( diagnostics::message(diagnostics::memory) )
  {
    std::stringstream msg;

    Address addrlimit = endAddress();

    msg << "  ***** Allocate Memory :: [" << _addr
        << ", " << addrlimit << ")"
        << "  size = " << _size
        << "  blocksize = " << blocksize
        << "  distributed = " << isDistributed()
        << "  @ (" << _pos.getLineInOrigFile() << ", " << _pos.getLineInTransformedFile() << ")";
    rs->printMessage(msg.str());
  }
}

void MemoryType::resize( size_t new_size )
{
    assert( new_size >= getSize() );
    initdata.resize( new_size );
}

MemoryType::Location MemoryType::endAddress() const
{
  return ::add(startAddress, getSize(), blockSize());
}

bool MemoryType::containsMemArea(Location queryAddress, size_t len) const
{
    std::ptrdiff_t ofs = byte_offset(startAddress, queryAddress, blockSize(), 0);

    return (ofs >= 0) && ((ofs+len) <= getSize());
}

MemoryType::Location MemoryType::lastValidAddress() const
{
  return ::add(startAddress, getSize()-1, blockSize());
}

bool MemoryType::containsAddress(Location queryAddr) const
{
    return containsMemArea(queryAddr, 1);
}

bool MemoryType::operator< (const MemoryType& other) const
{
    return (startAddress < other.startAddress);
}


bool MemoryType::isInitialized(Location addr, size_t len) const
{
    const size_t           offset = byte_offset(startAddress, addr, blockSize(), 0);
    const size_t           limit = offset + len;

    for (size_t i = offset; i < limit; ++i)
    {
        if ( !initdata[i] ) return false;
    }

    return true;
}

bool MemoryType::initialize(Location addr, size_t len)
{
    const size_t offset = byte_offset(startAddress, addr, blockSize(), 0);
    const size_t limit = offset + len;
    bool         statuschange = false;

    for (size_t i = offset; i < limit; ++i)
    {
      if (!initdata[i])
      {
        statuschange = true;
        initdata[i] = true;
      }
    }

    return statuschange;
}

static inline
void insert( MemoryType::TypeData& tmap, const MemoryType::TypeData::value_type& val )
{
  tmap.insert(val);
}

bool MemoryType::registerMemType(Location addr, size_t ofs, const RsType* type)
{
    // \pp addr = beginAddress() + ofs
    //     however, in UPC the conversion is quite involved when shared
    //     memory and blocking factors are in use.
    const std::pair<bool, bool> mergeRes = checkAndMergeMemType(ofs, type);
    bool                        statuschange = mergeRes.second;

    if ( diagnostics::message(diagnostics::memory) )
    {
      std::stringstream msg;
      msg << "++ registerMemType at offset: " << ofs << " (base = " << startAddress << ")\n"
          << "++ type: " << type->getName() << '\n'
          << "++ types_merged: " << mergeRes;

      RuntimeSystem::instance()->printMessage(msg.str());
    }

    if ( !mergeRes.first )
    {
        // type wasn't merged, some previously unknown portion of this
        // MemoryType has been registered with type
        assert(type);

        insert( typedata, TypeData::value_type(ofs, type) );

        // since we have knowledge about the type in memory, we also need to update the
        // information about "dereferentiable" memory regions i.e. pointer
        PointerManager* pm = rtedRTS(this)->getPointerManager();

        pm->createPointer(addr, type, blockSize());
        statuschange = true;
    }

    if ( diagnostics::message(diagnostics::memory) )
    {
      RuntimeSystem::instance()->printMessage("   ++ registerMemType done.");
    }

    return statuschange;
}

void MemoryType::forceRegisterMemType(const RsType* type )
{
    TiIterPair type_range = ::getOverlappingTypeInfos( typedata, 0, type->getByteSize() );

    typedata.erase(type_range.first, type_range.second);
    insert(typedata, TypeData::value_type(0, type));
}

static
std::string composeVioMessage(MemoryType::LocalPtr newTiStart, size_t len, const RsType* t, const MemoryType& mt)
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

std::pair<bool, bool>
MemoryType::checkAndMergeMemType(Location addr, const RsType* type)
{
  const size_t ofs = byte_offset(startAddress, addr, blockSize(), 0);

  return checkAndMergeMemType(ofs, type);
}


static inline
bool isDistMem(const MemoryType& mt, const RsType& t)
{
  // std::cout << "  #@@ " << typeid(t).name() << std::endl;
  return mt.isDistributed() && (dynamic_cast< const RsArrayType* >( &t ) != NULL);
}

static
std::pair<bool, bool> mtyFailed()
{
  return std::make_pair(false /*success*/, false /* no status update */);
}

static
std::pair<bool, bool> mtyNoUpdate()
{
  return std::make_pair(true /*success*/, true /* no status update */);
}

static
std::pair<bool, bool> mtyMerged()
{
  return std::make_pair(true /*success*/, true /* no status update */);
}


std::pair<bool, bool>
MemoryType::checkAndMergeMemType(size_t ofs, const RsType* type)
{
    if (typedata.size() == 0) // no types registered yet
        return mtyFailed();

    size_t            newStart = ofs;
    size_t            newLength = type->getByteSize();

    if ( diagnostics::message(diagnostics::memory) )
    {
      std::stringstream msg;

      msg << "++ checkAndMergeMemType ofs: " << newStart << " (base = " << startAddress << ")"
          << "   length: " << newLength << std::endl;

      RuntimeSystem::instance()->printMessage( msg.str() );
    }

    // Get a range of entries which overlap with [newStart,newTiEnd)
    TiIterPair res = ::getOverlappingTypeInfos(typedata, newStart, newLength);
    TiIter     itLower = res.first;
    TiIter     itUpper = res.second;

    // Case 0: There are types registered, but not ϵ [newStart, newTiEnd)
    if (itLower == itUpper)
      return mtyFailed();

    //  Case 1: New entry is overlapped by one old entry,
    //  i.e. [itLower,itUpper) contains only one entry
    TiIter     incrementedLower = itLower;
    ++incrementedLower;

    if (incrementedLower == itUpper)
    {
        const RsType*  oldType = itLower->second;
        size_t         oldStart = itLower->first;
        size_t         oldLength = oldType->getByteSize();
        std::ptrdiff_t diff = oldStart - newStart;

        // was: if( (oldStart <= newStart) && (oldTiEnd >= newTiEnd) )
        if ((diff <= 0) && (newLength <= (diff + oldLength)))
        {
            //Check if new entry is consistent with old one
            bool new_type_ok = oldType->checkSubtypeRecursive( newStart - oldStart, type );

            if( !new_type_ok )
            {
                // \todo \ofs startAddress.local + newStart
                RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS, composeVioMessage(startAddress.local + newStart, newLength, type, *this) );

                vio.descStream() << "Previously registered Type completely overlaps new Type in an inconsistent way:"
                                 << std::endl
                                 << "Containing Type " << oldType->getName()
                                 << " (" << oldStart << ","
                                 << (oldStart + oldLength) << ")  base = " << startAddress
                                 << std::endl;

                RuntimeSystem::instance()->violationHandler(vio);
                return mtyFailed();
            }
            else
            {
                // successful, no need to add because the type is already contained in bigger one
                // effectively type has already been “merged”.
                return mtyNoUpdate();
            }
        }
    }

    // Case 2: Iterate over overlapping old types and check consistency
    for(TypeData::iterator i = itLower; i != itUpper; ++i )
    {
        const RsType*  curType = i->second;
        size_t         curStart = i->first;
        size_t         curLength = curType->getByteSize();
        std::ptrdiff_t diff = curStart - newStart;

        // std::cout << "  >> " << diff << " #" << newLength << "  #" << curLength << std::endl;

        // was: if ((curStart < newStart) || (newTiEnd < curEnd))
        if ((diff < 0) || (newLength < (diff + curLength)))
        {
            // \todo \ofs startAddress.local + newStart
            RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS, composeVioMessage(startAddress.local + newStart, newLength, type, *this) );

            vio.descStream() << "Previously registered Type overlaps new Type in an inconsistent way:"
                             << std::endl
                             << "Overlapping Type " << curType->getName()
                             << " (" << curStart << "," << (curStart + curLength) << ")  base = " << startAddress
                             << std::endl;
            RuntimeSystem::instance()->violationHandler(vio);
            return mtyFailed();
        }

        const RsType* sub = type->getSubtypeRecursive(diff /*curStart - newStart*/, curLength);

        if (sub != curType)
        {
            // \todo \ofs startAddress.local + newStart
            RuntimeViolation vio(RuntimeViolation::INVALID_TYPE_ACCESS, composeVioMessage(startAddress.local + newStart, newLength, type, *this) );

            vio.descStream() << "Newly registered Type completely overlaps a previous registered Type in an inconsistent way:"
                             << std::endl
                             << "Overlapping Type " << curType->getName()
                             << " (" << curStart << "," << (curStart + curLength) << ")  base = " << startAddress
                             << std::endl;

            RuntimeSystem::instance()->violationHandler(vio);
            return mtyFailed();
        }
    }
    //All consistent - old typeInfos are replaced by big new typeInfo (merging)
    typedata.erase(itLower, itUpper);
    insert( typedata, TypeData::value_type(ofs, type) );

    // if we have knowledge about the type in memory, we also need to update the
    // information about "dereferentiable" memory regions i.e. pointer

    const long     blocksize = blockSize();
    Location       addr = ::add(startAddress, ofs, blocksize);
    RuntimeSystem* rts = rtedRTS(this);

    rts->getPointerManager()->createPointer(addr, type, blocksize);
    return mtyMerged();
}


const RsType* MemoryType::getTypeAt(size_t ofs, size_t size) const
{
    typedef TypeData::const_iterator      Iterator;
    typedef std::pair<Iterator, Iterator> IterPair;

    IterPair        type_range = ::getOverlappingTypeInfos(typedata, ofs, size);
    Iterator        first_addr_type = type_range.first;
    Iterator        end = type_range.second;

    //~ std::cerr << "   distance = " << std::distance(first_addr_type, end) << std::endl;
    //~ std::cerr << "   addr = " << static_cast<const void*>(addr.local)
              //~ << "  first = " << static_cast<const void*>(first_addr_type->first) << std::endl;

    if( first_addr_type == end )
    {
        // We know nothing about the types at offset..+size
        return &RsType::UnknownType;
    }

    const RsType*  res = NULL;
    Iterator       second_addr_type = first_addr_type;

    ++second_addr_type;
    // \pp   why can there be a number of overlapping typeinfos?
    //       possibly when continuing on an erroneous state?
    // \todo assert(second_addr_type == end)

    if ( second_addr_type == end && ofs >= first_addr_type->first )
    {
      // We know only something about one type in this range
      const RsType* first_type = first_addr_type->second;
      assert(first_type);

      // \pp I do not think that the third argument is very helpful
      //     In particular with multi-dimensional arrays, the returned
      //     array would not always be the top level
      res = first_type->getSubtypeRecursive( ofs, size, true /* stop-on-array */ );

      //~ std::cerr << "   res = " << static_cast<const void*>(res)
                //~ << "   " << typeid(*first_type).name() << std::endl;
    }

    return res;
}

const RsCompoundType* MemoryType::computeCompoundTypeAt(size_t ofs, size_t size) const
{
    typedef std::pair<TypeData::const_iterator, TypeData::const_iterator> ConstIterPair;

    // \pp not sure why this is needed? should not we always have
    //     type information of a memory location + length?
    //     -> no, not for C style dynamically allocated memory
    ConstIterPair         type_range = ::getOverlappingTypeInfos( typedata, ofs, size );

    // We don't know the complete type of the range, but we do know something.
    // Construct a compound type with the information we have.
    RsCompoundType* computed_type = new RsCompoundType( "Partially Known Type", size );

    for( TypeData::const_iterator i = type_range.first; i != type_range.second; ++i)
    {
        //~ std::stringstream out;
        //~ out << rted_ThisThread() << "  first: " << static_cast<const void*>(i->first) << "   addr: " << static_cast<const void*>(addr.local) << " " << (i->first <= addr.local) << "\n";
        //~ std::cerr << out.str() << std::flush;

        // \bug \pp this seems a bug in the RTS implementation
        // addr and i can overlap in a way that addr can be either smaller
        //   or larger then i->first. Thus the following assert does not hold.
        // assert(i->first >= addr.local);
        if (i->first >= ofs)
        {
          size_t  subtype_offset = i->first - ofs;
          const RsType* type = i -> second;

          //~ std::cerr << "  add member " << type->getName() << " @ " << subtype_offset << std::endl;
          computed_type->addMember( "", type, subtype_offset );
        }
    }

    return computed_type;
}


static
size_t basetypeSize(const RsArrayType& t)
{
  const RsType*      base = NULL;
  const RsArrayType* arr = &t;

  do
  {
    base = arr->getBaseType();
    arr = dynamic_cast<const RsArrayType*>(base);
  } while (arr);

  return base->getByteSize();
}

static
size_t basetypeSize(const RsType* t)
{
  const RsArrayType* arrt = dynamic_cast<const RsArrayType*>(t);

  if (!arrt) return 0;
  return basetypeSize(*arrt);
}

static
MemoryType::InitStatus initializationStatus(const MemoryType::InitData& initdata)
{
  assert(initdata.size() > 0);

  bool         allset   = true;
  bool         allunset = true;
  const size_t last = initdata.size();
  size_t       i = 0;

  while (i < last && (allset || allunset))
  {
    const bool isset = initdata[i];

    allset   = allset && isset;
    allunset = allunset && !isset;

    ++i;
  }

  assert(!(allset && allunset));

  if (allset)   return MemoryType::all;
  if (allunset) return MemoryType::none;
  return MemoryType::some;
}



std::string MemoryType::getInitString() const
{
    const char* res = NULL;

    switch (::initializationStatus(initdata))
    {
      case none: res = "Not initialized  "; break;
      case some: res = "Part. initialized"; break;
      case all:  res = "Initialized      "; break;
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
void MemoryType::print(std::ostream& os) const
{
    os << startAddress
       << " Size " << std::dec << getSize()
       <<  "\t" << getInitString() << "\tAllocated here: " << allocPos
       << std::endl;

    rtedRTS(this)->getPointerManager()->printPointersToThisChunk(os, *this);

    if (typedata.size() > 0)
    {
        os << "\t" << "Type Info" <<std::endl;
        for(TypeData::const_iterator i = typedata.begin(); i != typedata.end(); ++i)
        {
            os << "\t" << i->first << "\t" << i->second->getName() <<std::endl;
        }
    }
}

std::ostream& operator<< (std::ostream& os, const MemoryType& m)
{
    m.print(os);
    return os;
}


// -----------------------    MemoryManager  --------------------------------------

MemoryManager::~MemoryManager()
{
  checkForNonFreedMem();
}


/// \brief templated implementation of findPossibleMemMatch
///        to propagte the constness from the memory set to the result type
template <class MemTypeSet>
static
typename ConstLike<MemTypeSet, MemoryType>::type*
findPossibleMemMatch(MemTypeSet& mem, Address addr)
{
  // \pp \todo seems not to take into account different UPC blocksizes
  typedef typename ContainerTraits<MemTypeSet>::Iterator iterator;

  if (mem.size()==0)
    return NULL;

  // search the memory chunk, we get the chunk which has equal or greater startAddress
  iterator it = mem.lower_bound(addr);

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

MemoryType* MemoryManager::findPossibleMemMatch(Location addr)
{
  return ::findPossibleMemMatch(mem, addr);
}

const MemoryType*
MemoryManager::findPossibleMemMatch(Location addr) const
{
  return ::findPossibleMemMatch(mem, addr);
}

template <class MemType>
static
MemType* validateMembership(MemType* mt, Address addr, size_t size)
{
  return (mt && mt->containsMemArea(addr,size)) ?  mt  : NULL;
}

MemoryType* MemoryManager::findContainingMem(Location addr, size_t size)
{
    return ::validateMembership(findPossibleMemMatch(addr), addr, size);
}

const MemoryType*
MemoryManager::findContainingMem(Location addr, size_t size) const
{
    return ::validateMembership(findPossibleMemMatch(addr), addr, size);
}

bool MemoryManager::existOverlappingMem(Location addr, size_t size, long blocksize) const
{
    const MemoryType* res = findPossibleMemMatch(addr);

    // false, when no possible match exists
    if (!res) return false;

    // true, if res contains addr or the end-address
    return (  res->containsAddress(addr)
           || res->containsAddress(::add(addr, size, blocksize))
           );
}


MemoryType* MemoryManager::allocateMemory(Location adrObj, size_t szObj, MemoryType::AllocKind kind, long blocksize, const SourcePosition& pos)
{
    if (szObj == 0)
    {
        RuntimeSystem::instance()->violationHandler(RuntimeViolation::EMPTY_ALLOCATION,"Tried to call malloc/new with size 0\n");
        return NULL;
    }

    if (existOverlappingMem(adrObj, szObj, blocksize))
    {
        // the start address of new chunk lies in already allocated area
        RuntimeSystem::instance()->violationHandler(RuntimeViolation::DOUBLE_ALLOCATION);
        return NULL;
    }

    MemoryType                tmp(adrObj, szObj, kind, blocksize, pos);
    MemoryTypeSet::value_type v(adrObj, tmp);

    return &mem.insert(v).first->second;
}

static
std::string allocDisplayName(AllocKind ak)
{
  const char* res = NULL;

  switch (ak)
  {
    case akStack          : res = "Stack (function scope)"; break;
    case akGlobal         : res = "Global (file scope)"; break;
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
    case akCHeap          : res = "free"; break;

    /* C++ */
    case akCxxNew         : res = "operator delete"; break;
    case akCxxArrayNew    : res = "operator delete[]"; break;

    /* UPC */
    case akUpcShared      : res = "upc_free"; break;
    default               : res = "unknown deallocation method (rted bug?)";
  }

  assert(res != NULL);
  return res;
}

static
std::string
alloc_free_mismatch(MemoryType& m, AllocKind howAlloced, AllocKind howFreed)
{
  std::stringstream desc;

  if (howAlloced & (akStack | akGlobal))
  {
    desc << allocDisplayName(static_cast<AllocKind>(howAlloced & (akStack | akGlobal)))
         << " memory was explicitly freed (0x" << m << ") with "
         << freeDisplayName(howFreed)
         << std::endl;
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

static
void checkDeallocationAddress(MemoryType* m, MemoryType::Location addr, MemoryType::AllocKind freekind)
{
    // free on unknown address
    if (m == NULL)
    {
        RuntimeSystem*    rs = RuntimeSystem::instance();
        std::stringstream desc;

        desc << freeDisplayName(freekind);
        desc << " was called with unknown address " << addr <<std::endl;
        desc << " Allocated Memory Regions:" << std::endl;
        // MemoryManager::print(desc);
        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    }

    // free inside an allocated memory block
    if (m->beginAddress() != addr)
    {
        RuntimeSystem*    rs = RuntimeSystem::instance();
        std::stringstream desc;

        desc << freeDisplayName(freekind);
        desc << " was called with an address inside of an allocated block (Address:" << addr <<")" <<std::endl;
        desc << "Allocated Block: " << *m <<std::endl;

        rs->violationHandler(RuntimeViolation::INVALID_FREE, desc.str());
        return;
    }
}

void MemoryManager::freeMemory(MemoryType* m, MemoryType::AllocKind freekind)
{
    // free stack memory explicitly (i.e. not via exitScope)
    MemoryType::AllocKind allockind = m->howCreated();

    // allockind can be different from freekind
    //   e.g., upc_free subsumes all upc allocations
    // (1) same method used to allocate and free
    // (2) no free of global memory
    if ( (allockind & (freekind | akGlobal | akStack)) != freekind )
    {
      RuntimeSystem* rs = RuntimeSystem::instance();
      std::string    msg = alloc_free_mismatch(*m, allockind, freekind);

      rs->violationHandler(RuntimeViolation::INVALID_FREE, msg);
      return;
    }

    PointerManager* pm = rtedRTS(this)->getPointerManager();

    pm->deletePointerInRegion( *m );
    pm->invalidatePointerToRegion( *m );

    // successful free, erase allocation info from map
    mem.erase(m->beginAddress());
}


void MemoryManager::freeHeapMemory(Location addr, MemoryType::AllocKind freekind)
{
    MemoryType* mt = findContainingMem(addr, 1);

    checkDeallocationAddress(mt, addr, freekind);
    freeMemory( findContainingMem(addr, 1), freekind );
}

void MemoryManager::freeStackMemory(Location addr)
{
    MemoryType* mt = findContainingMem(addr, 1);

    checkDeallocationAddress(mt, addr, akStack);
    freeMemory( findContainingMem(addr, 1), mt->howCreated() );
}


template <class MemManager>
static
typename ConstLike<MemManager, MemoryType>::type*
checkLocation(MemManager& mgr, Address addr, size_t size, RuntimeViolation::Type vioType)
{
  typedef typename ConstLike<MemManager, MemoryType>::type ResultType;

  ResultType* res = mgr.findContainingMem(addr, size);

  if (!res)
  {
      RuntimeSystem*    rs = RuntimeSystem::instance();
      std::stringstream desc;
      desc << "Trying to access non-allocated MemoryRegion "
           << "(Address " << addr << " of size " << std::dec << size << ")"
           << std::endl;

      if (const MemoryType* possMatch = mgr.findPossibleMemMatch(addr))
      {
          desc << "Did you try to access this region:" << std::endl << *possMatch << std::endl;
      }

      rs->violationHandler(vioType, desc.str());
  }

  return res;
}

const MemoryType*
MemoryManager::checkLocation(Location addr, size_t size, RuntimeViolation::Type vioType) const
{
  return ::checkLocation(*this, addr, size, vioType);
}

void MemoryManager::checkRead(Location addr, size_t size) const
{
    const MemoryType* mt = checkLocation(addr, size, RuntimeViolation::INVALID_READ);
    assert(mt && ((mt->beginAddress() <= addr) || (!mt->blockSize())));

    if(! mt->isInitialized(addr, size) )
    {
      RuntimeSystem * rs = RuntimeSystem::instance();

      std::stringstream desc;
      desc << "Trying to read from uninitialized MemoryRegion "
           << "(Address " << addr
           << " of size " << std::dec << size << ")"
           << std::endl;

      rs->violationHandler(RuntimeViolation::INVALID_READ, desc.str());
    }
}

std::pair<MemoryType*, bool>
MemoryManager::checkWrite(Location addr, size_t size, const RsType* t)
{
    if ( diagnostics::message(diagnostics::memory) )
    {
      std::stringstream msg;

      msg << "   ++ checkWrite: " << addr << " size: " << size;
      RuntimeSystem::instance()->printMessage(msg.str());
    }

    bool           statuschange = false;
    MemoryType*    mt = ::checkLocation(*this, addr, size, RuntimeViolation::INVALID_WRITE);

    // the address has to be inside the block
    assert(mt && mt->containsMemArea(addr, size));

    if (t)
    {
      if ( diagnostics::message(diagnostics::memory) )
      {
        RuntimeSystem*    rs = RuntimeSystem::instance();
        std::stringstream msg;

        msg << "++ found memory addr: " << *mt << " for " << addr << " with size " << ToString(size);
        rs->printMessage(msg.str());
      }

      const long   blocksize = mt->blockSize();
      const size_t ofs = byte_offset(mt->beginAddress(), addr, blocksize, 0);

      statuschange = mt->registerMemType(addr, ofs, t);
    }

    statuschange = mt->initialize(addr, size) || statuschange;

    if ( diagnostics::message(diagnostics::memory) )
    {
      RuntimeSystem::instance()->printMessage("   ++ checkWrite done.");
    }

    return std::make_pair(mt, statuschange);
}

bool MemoryManager::isInitialized(Location addr, size_t size) const
{
    const MemoryType* mt = checkLocation(addr, size, RuntimeViolation::INVALID_READ);

    return mt->isInitialized(addr, size);
}


bool MemoryManager::checkIfSameChunk(Location addr1, Location addr2, const RsType* type) const
{
  return checkIfSameChunk( addr1, addr2, type->getByteSize() );
}

#if WITH_UPC

static
std::ptrdiff_t byte_offset(size_t baseaddr, size_t basethread, size_t elemaddr, size_t elemthread)
{
  std::ptrdiff_t ofs = elemaddr - baseaddr;

  ofs *= rted_Threads();
  return ofs + (elemthread - basethread);
}


static
std::ptrdiff_t byte_offset(Address base, Address elem, size_t blocksize)
{
  assert(blocksize != 0);

  // adjust base so that it is aligned on a blocksize address
  const size_t         baseaddr  = reinterpret_cast<size_t>(base.local);
  const size_t         basescale = baseaddr / blocksize;
  const size_t         baseshift = baseaddr % blocksize;

  // adjust elem by the same amount
  const size_t         elemaddr  = reinterpret_cast<size_t>(elem.local - baseshift);
  const size_t         elemscale = elemaddr / blocksize;
  const size_t         elemphase = elemaddr % blocksize;

  // use simpler calculation offset calculation
  const std::ptrdiff_t scaleofs = byte_offset(basescale, base.thread_id, elemscale, elem.thread_id);

  // unscale and add back the elem's phase
  return scaleofs * blocksize + elemphase;
}

#endif /* WITH_UPC */

// declared static by preceeding declaration
std::ptrdiff_t byte_offset(Address base, Address elem, size_t blocksize, size_t blockofs)
{
  ez::unused(blocksize), ez::unused(blockofs);

#if WITH_UPC
  if (blocksize > 0)
  {
    base.local -= blockofs;
    return byte_offset(base, elem, blocksize) - blockofs;
  }
#endif /* WITH_UPC */

  assert(blocksize == 0 && blockofs == 0);
  return elem.local - base.local;
}

bool
MemoryManager::checkIfSameChunk(Location addr1, Location addr2, size_t typeSize, RuntimeViolation::Type violation) const
{
  RuntimeViolation::Type access_violation = violation;

  if (access_violation != RuntimeViolation::NONE) access_violation = RuntimeViolation::INVALID_READ;

  const MemoryType*      mem1 = checkLocation(addr1, typeSize, access_violation);
  const bool             sameChunk = (mem1 && mem1->containsMemArea(addr2, typeSize));

  // check that addr1 and addr2 point to the same base location
  if (! sameChunk)
  {
    const MemoryType*    mem2 = checkLocation(addr2, typeSize, access_violation);
    // the error is skipped, if a chunk is not available b/c pointer errors
    // should be recorded only when the out-of-bounds memory is accessed, but
    // not when the pointer is moved out of bounds.
    const bool           skiperr = (violation == RuntimeViolation::NONE && !(mem1 && mem2));

    if (!skiperr)
    {
      assert(mem1 && mem2 && mem1 != mem2);
      RuntimeSystem*       rs = RuntimeSystem::instance();
      std::stringstream    ss;

      ss << "Pointer changed allocation block from " << addr1 << " to " << addr2 << std::endl;

      rs->violationHandler( violation, ss.str() );
    }

    return false;
  }

  // so far we know that addr1, addr2 have been allocated within the same chunk
  // now, test for same array chunks
  const size_t                totalblocksize = mem1->blockSize();
  const Location              memloc = mem1->beginAddress();
  const std::ptrdiff_t        ofs1 = byte_offset(memloc, addr1, totalblocksize, 0);
  const std::ptrdiff_t        ofs2 = byte_offset(memloc, addr2, totalblocksize, 0);

  //~ std::cerr << *mem1 << std::endl;
  //~ std::cerr << "a = " << addr1 << std::endl;
  //~ std::cerr << "b = " << addr2 << std::endl;
  //~ std::cerr << "ts = " << typeSize << std::endl;
  //~ std::cerr << "tbs = " << totalblocksize << std::endl;

  // \pp as far as I understand, getTypeAt returns the innermost type
  //     that overlaps a certain memory region [addr1, typeSize).
  //     The current implementation stops at the highest level of array...
  const RsType*               type1 = mem1 -> getTypeAt( ofs1, typeSize );
  const RsType*               type2 = mem1 -> getTypeAt( ofs2, typeSize );
  std::auto_ptr<const RsType> guard1(NULL);
  std::auto_ptr<const RsType> guard2(NULL);

  if( !type1 ) {
      //~ std::cerr << "type1! " << std::endl;
      type1 = mem1->computeCompoundTypeAt( ofs1, typeSize );
      guard1.reset(type1);
  }

  if( !type2 ) {
      //~ std::cerr << "type2! " << std::endl;
      type2 = mem1->computeCompoundTypeAt( ofs2, typeSize );
      guard2.reset(type2);
  }

  assert( type1 && type2 );

  //~ std::cerr << "-- T1 = " << typeid(*type1).name() << std::endl;
  //~ std::cerr << "-- T2 = " << typeid(*type2).name() << std::endl;

  bool               accessOK = type1->isConsistentWith( *type2 );
  const RsArrayType* array = (accessOK ? dynamic_cast< const RsArrayType* >( type1 ) : 0);

  //~ std::cerr << "accOK: " << accessOK << std::endl;

  if (array)
  {
    // \pp in order to calculate the last memory location of elem, we need to
    //     know whether its type is an array type, and whether this is an
    //     immediate (there is no user defined type or pointer in between)
    //     subarray of the containing array.
    const size_t          blockofs = 0; /* \todo calculate offset from addr1 */

    // \pp \todo
    //     not sure why bounds checking is based on a relative address (addr1)
    //     and not on absolute boundaries of the chunk where this address
    //     is located...
    //     e.g., addr2 < array(addr).lb || array(addr).ub < (addr2+typeSize)
    //     - a reason is that we want to bounds check within a multi-dimensional
    //       array; a sub-array might not start at the same address as the
    //       allocated chunk (which is always at [0][0]...)
    // \bug  this works fine for arrays, but it does not seem to be OK for
    //       pointers; in which case addr1 might point in the middle of a chunk.
    const Location       arrlb = addr1;
    const std::ptrdiff_t arrlen = array->getByteSize();
    const Location       elemlo = addr2;
          Location       elemhi = addr2;
    const RsType*        memtype = mem1->getTypeAt( 0, mem1->getSize() );
    const size_t         basesize = basetypeSize(memtype);

    if (typeSize > basesize)
    {
      // elem is another array
      // assert( rs->testing() || dynamic_cast< const RsArrayType* >(type2) );

      const long   elemblockofs = 0; // \todo calculate the ofs of elem's address (is this needed?)

      elemhi = ::add(elemhi, typeSize-1, totalblocksize, elemblockofs);
    }
    else
    {
      elemhi.local += (typeSize-1);
    }

    // \pp \note [arrlb, arrub] and [elemlo, elemhi]  (therefore typeSize - 1)
    //           arrub < elemhi this holds for UPC
    //           in contrast, using a range [elemlb, elemub) causes problems
    //           for UPC where the elemub can be larger than arrub
    //           for last elements on threads other than 0.
    //           e.g., shared int x[THREADS];
    //                   arrub == &x@thread(0) + sizeof(int)
    //                 writing to x[1] has the following upper bound
    //                   elemub == &x@thread(1) + sizeof(int)
    //                 consequently arrub < elemub :(

#if EXTENDEDDBG
    std::cerr << "  arrlb = " << arrlb << "( lb + " << arrlen << " [" << totalblocksize << "] )" << std::endl;
    std::cerr << "  elmlb = " << elemlo << std::endl;
    std::cerr << "  elmub = " << elemhi << " ( lb + " << typeSize << " )" << std::endl;

    std::cerr << "  ofs(elemlb) >= 0 : " << byte_offset(arrlb, elemlo, totalblocksize, blockofs) << std::endl;
    std::cerr << "  ofs(elemub) < sz(arr) : " << (byte_offset(arrlb, elemhi, totalblocksize, blockofs)) << std::endl;
#endif /* EXTENDEDDBG */

    // the offset of the element in respect to the array has to be positive
    //   and the offset of the elements last byte has to be less than the
    //   array length.
    accessOK  = (byte_offset(arrlb, elemlo, totalblocksize, blockofs) >= 0);
    accessOK &= (byte_offset(arrlb, elemhi, totalblocksize, blockofs) < arrlen);

    // the array element might be before it [ -1 ]
    // ... or after the array [ N ]...
    // was: accessOK = ! ( (elemlo < arrlb) || (arrub < elemhi) )
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
    failNotSameChunk( *type1, *type2, addr1, addr2, *mem1, violation );

  return accessOK;
}

void MemoryManager::failNotSameChunk( const RsType& type1,
                                      const RsType& type2,
                                      Location addr1,
                                      Location addr2,
                                      const MemoryType& mem1,
                                      RuntimeViolation::Type violation
                                    ) const
{
    RuntimeSystem *   rs = RuntimeSystem::instance();
    std::stringstream ss;

    ss << "A pointer changed the memory area (subarray or variable) which it points to (may be an error)"
       << std::endl << std::endl
       << "Mem region:  " << mem1 << std::endl
       << "  addr1 = " << addr1 << "  type = " << type1 << std::endl
       << "  addr2 = " << addr2 << "  type = " << type2 << std::endl;

    rs->violationHandler( violation, ss.str() );
}


void MemoryManager::checkForNonFreedMem() const
{
    if (mem.size() !=0)
    {
        std::stringstream desc;

        desc << "Program terminated and the following allocations were not freed:" <<std::endl;
        print(desc);

        RuntimeSystem::instance()->violationHandler(RuntimeViolation::MEMORY_LEAK, desc.str());
    }
}


template <class MemType>
MemType* checkStartAddress(MemType* mt, Address addr)
{
  if (!mt || (mt->beginAddress() != addr)) return NULL;
  return mt;
}

MemoryType* MemoryManager::getMemoryType(Location addr)
{
  return ::checkStartAddress( findPossibleMemMatch(addr), addr );
}

const MemoryType* MemoryManager::getMemoryType(Location addr) const
{
  return ::checkStartAddress( findPossibleMemMatch(addr), addr );
}


void MemoryManager::print(std::ostream& os) const
{
    if (mem.size() == 0)
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
